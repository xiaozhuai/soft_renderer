//
// Created by xiaozhuai on 2021/12/30.
//

#include "Texture2D.h"

#include <stb_image.h>

template<typename T>
inline T clamp(T value, T low, T high) {
    return (value < low) ? low : ((value > high) ? high : value);
}

Texture2D::Texture2D(int width, int height, const uint8_t *data) {
    resize();
    if (data == nullptr) {
        memset(m_pixels.get(), 0, size());
    } else {
        memcpy(m_pixels.get(), data, size());
    }
}

Texture2D::Texture2D(const std::string &file, bool flipVertically) {
    load(file, flipVertically);
}

bool Texture2D::load(const std::string &file, bool flipVertically) {
    int w, h, c;
    if (flipVertically) {
        stbi_set_flip_vertically_on_load(flipVertically);
    }
    auto *data = stbi_load(file.c_str(), &w, &h, &c, 4);
    if (data == nullptr) {
        m_width = 0;
        m_height = 0;
        resize();
        return false;
    }
    m_width = w;
    m_height = h;
    resize();
    memcpy(m_pixels.get(), data, size());
    return valid();
}

Colorf Texture2D::texture(const glm::vec2 &uv) const {
    if (!valid()) {
        return {0.0f, 0.0f, 0.0f, 0.0f};
    }
    auto newUV = uv;
    newUV.x = clamp(newUV.x, 0.0f, 1.0f);
    newUV.y = clamp(newUV.y, 0.0f, 1.0f);
    int x = (int) std::round(newUV.x * float(m_width));
    int y = (int) std::round(newUV.y * float(m_height));
    const uint8_t *p = m_pixels.get() + (m_width * y + x) * 4;
    return {
            float(p[0]) / 255.0f,
            float(p[1]) / 255.0f,
            float(p[2]) / 255.0f,
            float(p[3]) / 255.0f
    };
}
