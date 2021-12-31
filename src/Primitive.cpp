//
// Created by xiaozhuai on 2021/12/29.
//

#include "Primitive.h"

glm::vec3i world2screen(int width, int height, int depth, const glm::vec3 &p) {
    return {
            (int) std::round((p[0] + 1.0) * width / 2.0),
            (int) std::round((-p[1] + 1.0) * height / 2.0),
            (int) std::round((p[2] + 1.0) * depth / 2.0)
    };
}

void drawLine(
        Framebuffer &framebuffer,
        const std::array<glm::vec3, 2> &pts,
        const Colorf &color) {
    std::array<glm::vec3i, 2> ptsi = {
            world2screen(framebuffer.width(), framebuffer.height(), framebuffer.depth(), pts[0]),
            world2screen(framebuffer.width(), framebuffer.height(), framebuffer.depth(), pts[1])
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
            framebuffer.set(y, x, color);
        } else {
            framebuffer.set(x, y, color);
        }

        eps += dy;
        // 这里用位运算 <<1 代替 *2
        if ((eps << 1) >= dx) {
            y = y + yi;
            eps -= dx;
        }
    }
}

glm::vec3 barycentric(const glm::vec3i &v1, const glm::vec3i &v2, const glm::vec3i &v3, const glm::vec3i &p) {
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

void drawTriangle(
        Framebuffer &framebuffer,
        const std::array<glm::vec3, 3> &pts,
        const std::array<glm::vec2, 3> &pts2,
        const std::array<glm::vec3, 3> &pts3,
        const glm::vec3 &lightDir,
        const Texture2D &texture) {
    std::array<glm::vec3i, 3> ptsi = {
            world2screen(framebuffer.width(), framebuffer.height(), framebuffer.depth(), pts[0]),
            world2screen(framebuffer.width(), framebuffer.height(), framebuffer.depth(), pts[1]),
            world2screen(framebuffer.width(), framebuffer.height(), framebuffer.depth(), pts[2])
    };

    // 找出包围盒
    glm::vec2i boxmin(framebuffer.width() - 1, framebuffer.height() - 1);
    glm::vec2i boxmax(0, 0);
    glm::vec2i clamp(framebuffer.width() - 1, framebuffer.height() - 1); // 边界

    for (int i = 0; i < 3; ++i) {
        boxmin.x = std::max(0, std::min(boxmin.x, ptsi[i].x));
        boxmin.y = std::max(0, std::min(boxmin.y, ptsi[i].y));
        boxmax.x = std::min(clamp.x, std::max(boxmax.x, ptsi[i].x));
        boxmax.y = std::min(clamp.y, std::max(boxmax.y, ptsi[i].y));
    }

    // auto n = glm::normalize(glm::cross(pts[2] - pts[0], pts[1] - pts[0]));
    // float intensity = glm::dot(n, lightDir);

    // 包围盒内的每一个像素判断在不在三角形内
#pragma omp parallel for
    for (int x = boxmin.x; x <= boxmax.x; ++x) {
        for (int y = boxmin.y; y <= boxmax.y; ++y) {
            glm::vec3i p;
            p.x = x;
            p.y = y;

            auto bc_screen = barycentric(ptsi[0], ptsi[1], ptsi[2], p);
            if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0) {
                continue;
            }

            p.z = (int) std::round(
                    float(ptsi[0].z) * bc_screen[0]
                    + float(ptsi[1].z) * bc_screen[1]
                    + float(ptsi[2].z) * bc_screen[2]
            );
            glm::vec2 uv = pts2[0] * bc_screen[0] + pts2[1] * bc_screen[1] + pts2[2] * bc_screen[2];
            glm::vec3 n = pts3[0] * bc_screen[0] + pts3[1] * bc_screen[1] + pts3[2] * bc_screen[2];
            // TODO 为什么是负的？
            n = -glm::normalize(n);
            float intensity = glm::dot(n, lightDir);

            if (p.z >= std::numeric_limits<uint16_t>::min()
                && p.z <= std::numeric_limits<uint16_t>::max()
                && p.z >= framebuffer.getDepth(p.x, p.y)
                && intensity >= 0) {

                Colorf color = texture.texture(uv);
                color.r *= intensity;
                color.g *= intensity;
                color.b *= intensity;

                framebuffer.setDepth(p.x, p.y, p.z);
                framebuffer.set(p.x, p.y, color);
            }
        }
    }
}
