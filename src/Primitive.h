//
// Created by xiaozhuai on 2021/12/29.
//

#ifndef SOFT_RENDERER_PRIMITIVE_H
#define SOFT_RENDERER_PRIMITIVE_H

#include "Framebuffer.h"
#include "Texture2D.h"
#include "Types.h"
#include <unordered_map>

ScreenPos world2screen(int width, int height, int depth, const glm::vec4 &p);

void primitiveTriangles(int width, int height,
                               const std::vector<glm::vec4> &positions,
                               const std::function<void(int, const std::array<ScreenPos, 3> &)> &func);

void rasterizationTriangle(int width, int height,
                                  int offset,
                                  const std::array<ScreenPos, 3> &pts,
                                  const std::unordered_map<std::string, AVData> &varyings,
                                  const std::function<void(const ScreenPos &, const std::unordered_map<std::string, glm::vec4> &)> &func);

#endif //SOFT_RENDERER_PRIMITIVE_H
