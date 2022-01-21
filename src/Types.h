//
// Created by xiaozhuai on 2021/12/30.
//

#ifndef SOFT_RENDERER_TYPES_H
#define SOFT_RENDERER_TYPES_H

#include <array>
#include <string>
#include <memory>
#include <utility>
#include <glm/glm.hpp>

namespace glm {
using vec4i = glm::vec<4, int>;
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

struct AttributeInfo {
    int size = 0;
    const float *data = nullptr;
};

struct AVData {
    inline void alloc(std::string _name, int _size, int _count) {
        if (data == nullptr || size * count < _size * _count) {
            data = std::shared_ptr<float>(new float[_size * _count], std::default_delete<float[]>());
        }
        name = std::move(_name);
        size = _size;
        count = _count;
    }

    inline size_t nFloats() const { return size * count; }

    inline size_t nBytes() const { return nFloats() * sizeof(float); }

    std::string name;
    int size = 0;
    int count = 0;
    std::shared_ptr<float> data;
};

#endif //SOFT_RENDERER_TYPES_H
