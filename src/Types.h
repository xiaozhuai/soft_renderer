//
// Created by xiaozhuai on 2021/12/30.
//

#ifndef SOFT_RENDERER_TYPES_H
#define SOFT_RENDERER_TYPES_H

#include <array>
#include <glm/glm.hpp>

namespace glm {
using vec3i = glm::vec<3, int>;
using vec2i = glm::vec<2, int>;
}

using Colorf = glm::vec4;

union ColorBGRA8888 {
    ColorBGRA8888()
            : data{0, 0, 0, 0} {}

    ColorBGRA8888(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
            : data{b, g, r, a} {}

    uint32_t val;
    uint8_t data[4];
    struct {
        uint8_t b;
        uint8_t g;
        uint8_t r;
        uint8_t a;
    } pixel;
};

#endif //SOFT_RENDERER_TYPES_H
