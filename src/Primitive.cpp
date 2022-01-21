//
// Created by xiaozhuai on 2021/12/29.
//

#include "Primitive.h"
#include "Log.h"
#include <oneapi/tbb.h>

ScreenPos world2screen(int width, int height, const glm::vec4 &p) {
    // TODO position w
    return {
            .x = (int) std::round((p.x + 1.0) * width / 2.0),
            .y =  (int) std::round((-p.y + 1.0) * height / 2.0),
            .z = p.z,
            .w = p.w
    };
}

void drawLine(
        Framebuffer &framebuffer,
        const std::array<glm::vec4, 2> &pts,
        const Colorf &color) {
    std::array<ScreenPos, 2> ptsi = {
            world2screen(framebuffer.width(), framebuffer.height(), pts[0]),
            world2screen(framebuffer.width(), framebuffer.height(), pts[1])
    };

    int x1 = ptsi[0].x;
    int y1 = ptsi[0].y;
    int x2 = ptsi[1].x;
    int y2 = ptsi[1].y;
    // TODO zbuffer

    // 处理斜率的绝对值大于 1 的直线
    bool steep = false;
    if (std::abs(x1 - x2) < std::abs(y1 - y2)) {
        std::swap(x1, y1);
        std::swap(x2, y2);
        steep = true;
    }
    // 交换起点终点的坐标
    if (x1 > x2) {
        std::swap(x1, x2);
        std::swap(y1, y2);
    }
    int y = y1;
    int eps = 0;
    int dx = x2 - x1;
    int dy = y2 - y1;
    int yi = 1;

    // 处理 [-1, 0] 范围内的斜率
    if (dy < 0) {
        yi = -1;
        dy = -dy;
    }

    for (int x = x1; x <= x2; x++) {
        if (steep) {
            framebuffer.setColor(y, x, color);
        } else {
            framebuffer.setColor(x, y, color);
        }

        eps += dy;
        // 这里用位运算 <<1 代替 *2
        if ((eps << 1) >= dx) {
            y = y + yi;
            eps -= dx;
        }
    }
}

static glm::vec3 barycentric(const glm::vec2i &v1, const glm::vec2i &v2, const glm::vec2i &v3, const glm::vec2i &p) {
    glm::vec3i a(v2.x - v1.x, v3.x - v1.x, v1.x - p.x);
    glm::vec3i b(v2.y - v1.y, v3.y - v1.y, v1.y - p.y);
    // u 向量和 a b 向量的点积为 0，所以 a b 向量叉乘可以得到 u 向量
    glm::vec3i u(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
    // u.z 绝对值小于 1 意味着三角形退化了，需要舍弃
    if (std::abs(u.z) < 1) {
        return {-1.0f, 1.0f, 1.0f};
    }
    return {1.0f - float(u.x + u.y) / float(u.z), float(u.x) / float(u.z), float(u.y) / float(u.z)};
}

void primitiveTriangles(int width, int height, const std::vector<glm::vec4> &positions,
                        const std::function<void(int, const std::array<ScreenPos, 3> &)> &func) {
    int nTriangles = positions.size() / 3;
    oneapi::tbb::parallel_for(0, nTriangles, [&](int n) {
        std::array<ScreenPos, 3> pts = {
                world2screen(width, height, positions[n * 3]),
                world2screen(width, height, positions[n * 3 + 1]),
                world2screen(width, height, positions[n * 3 + 2]),
        };
        func(n * 3, pts);
    });
}

void rasterizationTriangle(int width, int height,
                           int offset,
                           const std::array<ScreenPos, 3> &pts,
                           const std::unordered_map<std::string, AVData> &varyings,
                           const std::function<void(const ScreenPos &,
                                                    const std::unordered_map<std::string, glm::vec4> &)> &func) {
    // 找出包围盒
    glm::vec2i boxmin(width - 1, height - 1);
    glm::vec2i boxmax(0, 0);
    glm::vec2i clamp(width - 1, height - 1); // 边界
    for (int i = 0; i < 3; ++i) {
        boxmin.x = std::max(0, std::min(boxmin.x, pts[i].x));
        boxmin.y = std::max(0, std::min(boxmin.y, pts[i].y));
        boxmax.x = std::min(clamp.x, std::max(boxmax.x, pts[i].x));
        boxmax.y = std::min(clamp.y, std::max(boxmax.y, pts[i].y));
    }

    // 包围盒内的每一个像素判断在不在三角形内

    oneapi::tbb::parallel_for(
            oneapi::tbb::blocked_range2d<int>(boxmin.y, boxmax.y + 1, boxmin.x, boxmax.x + 1),
            [&](oneapi::tbb::blocked_range2d<int> &r) {
                for (int y = r.rows().begin(), y_end = r.rows().end(); y < y_end; ++y) {
                    for (int x = r.cols().begin(), x_end = r.cols().end(); x < x_end; ++x) {
                        // TODO p.w
                        ScreenPos p{};
                        p.x = x;
                        p.y = y;

                        auto bc_screen = barycentric(pts[0].xy(), pts[1].xy(), pts[2].xy(), p.xy());
                        if (bc_screen.x >= 0 && bc_screen.y >= 0 && bc_screen.z >= 0) {
                            glm::vec3 bc_clip = {bc_screen.x / pts[0].w, bc_screen.y / pts[1].w,
                                                 bc_screen.z / pts[2].w};
                            bc_clip = bc_clip / (bc_clip.x + bc_clip.y + bc_clip.z);

                            p.z = glm::dot(glm::vec3(pts[0].z, pts[1].z, pts[2].z), bc_clip);
                            p.w = glm::dot(glm::vec3(pts[0].w, pts[1].w, pts[2].w), bc_clip);

                            // p.z = pts[0].z * bc_screen[0]
                            //               + pts[1].z * bc_screen[1]
                            //               + pts[2].z * bc_screen[2];
                            // p.w = pts[0].w * bc_screen[0]
                            //       + pts[1].w * bc_screen[1]
                            //       + pts[2].w * bc_screen[2];

                            std::unordered_map<std::string, glm::vec4> varyingsValue;
                            for (const auto &it: varyings) {
                                const std::string &name = it.first;
                                const auto &varying = it.second;
                                auto *ptr = varying.data.get() + offset * varying.size;
                                glm::mat<3, 4, float, glm::defaultp> pv{};
                                switch (varying.size) {
                                    case 1:
                                        pv[0] = glm::vec4(ptr[0], 0, 0, 0);
                                        pv[1] = glm::vec4(ptr[1], 0, 0, 0);
                                        pv[2] = glm::vec4(ptr[2], 0, 0, 0);
                                        break;
                                    case 2:
                                        pv[0] = glm::vec4(ptr[0], ptr[1], 0, 0);
                                        pv[1] = glm::vec4(ptr[2], ptr[3], 0, 0);
                                        pv[2] = glm::vec4(ptr[4], ptr[5], 0, 0);
                                        break;
                                    case 3:
                                        pv[0] = glm::vec4(ptr[0], ptr[1], ptr[2], 0);
                                        pv[1] = glm::vec4(ptr[3], ptr[4], ptr[5], 0);
                                        pv[2] = glm::vec4(ptr[6], ptr[7], ptr[8], 0);
                                        break;
                                    case 4:
                                        pv[0] = glm::vec4(ptr[0], ptr[1], ptr[2], ptr[3]);
                                        pv[1] = glm::vec4(ptr[4], ptr[5], ptr[6], ptr[7]);
                                        pv[2] = glm::vec4(ptr[8], ptr[9], ptr[10], ptr[11]);
                                        break;
                                    default:
                                        pv[0] = glm::vec4(0, 0, 0, 0);
                                        pv[1] = glm::vec4(0, 0, 0, 0);
                                        pv[2] = glm::vec4(0, 0, 0, 0);
                                        break;
                                }

                                varyingsValue[name] = pv * bc_clip;

                                // varyingsValue[name] =
                                //         pv[0] * bc_screen[0]
                                //         + pv[1] * bc_screen[1]
                                //         + pv[2] * bc_screen[2];
                            }
                            func(p, varyingsValue);
                        }
                    }
                }
            }
    );
}
