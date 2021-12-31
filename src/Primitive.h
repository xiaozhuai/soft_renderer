//
// Created by xiaozhuai on 2021/12/29.
//

#ifndef SOFT_RENDERER_PRIMITIVE_H
#define SOFT_RENDERER_PRIMITIVE_H

#include "Framebuffer.h"
#include "Texture2D.h"
#include "Types.h"

glm::vec3i world2screen(int width, int height, int depth, const glm::vec3 &p);

void drawLine(
        Framebuffer &framebuffer,
        const std::array<glm::vec3, 2> &pts,
        const Colorf &color);

void drawTriangle(
        Framebuffer &framebuffer,
        const std::array<glm::vec3, 3> &pts,
        const std::array<glm::vec2, 3> &pts2,
        const std::array<glm::vec3, 3> &pts3,
        const glm::vec3 &lightDir,
        const Texture2D &texture);

#endif //SOFT_RENDERER_PRIMITIVE_H
