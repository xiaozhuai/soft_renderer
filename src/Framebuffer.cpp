//
// Created by xiaozhuai on 2021/12/29.
//

#include "Framebuffer.h"
#include <cmath>
#include <stb_image_write.h>

template<typename T>
inline T clamp(T value, T low, T high) {
    return (value < low) ? low : ((value > high) ? high : value);
}

inline uint8_t f_to_u8(float value) {
    return (uint8_t) std::round(value * 255.0f);
}

inline float u8_to_f(uint8_t value) {
    return float(value) / 255.0f;
}

inline ColorBGRA8888 color_f2i(const Colorf &color) {
    return {
            f_to_u8(clamp(color[0], 0.0f, 1.0f)),
            f_to_u8(clamp(color[1], 0.0f, 1.0f)),
            f_to_u8(clamp(color[2], 0.0f, 1.0f)),
            f_to_u8(clamp(color[3], 0.0f, 1.0f))
    };
}

inline Colorf color_i2f(const ColorBGRA8888 &color) {
    return {
            u8_to_f(color.pixel.r),
            u8_to_f(color.pixel.g),
            u8_to_f(color.pixel.b),
            u8_to_f(color.pixel.a)
    };
}

Framebuffer::Framebuffer(int width, int height)
        : m_width(width),
          m_height(height),
          m_colorBuffer(width * height * 4),
          m_depthBuffer(width * height, 0) {}

void Framebuffer::resize(int width, int height) {
    m_width = width;
    m_height = height;
    m_colorBuffer.resize(width * height * 4);
    m_depthBuffer.resize(width * height);
}

void Framebuffer::set(int x, int y, const Colorf &color) {
    // assert(x >= 0);
    // assert(x < m_width);
    // assert(y >= 0);
    // assert(y < m_height);
    if (m_colorBuffer.empty() || x < 0 || x >= m_width || y < 0 || y >= m_height) {
        return;
    }
    m_colorBuffer[m_width * y + x] = color_f2i(color);
}

Colorf Framebuffer::get(int x, int y) const {
    // assert(x >= 0);
    // assert(x < m_width);
    // assert(y >= 0);
    // assert(y < m_height);
    if (m_colorBuffer.empty() || x < 0 || x >= m_width || y < 0 || y >= m_height) {
        return {0.0f, 0.0f, 0.0f, 0.0f};
    }
    return color_i2f(m_colorBuffer[m_width * y + x]);
}

void Framebuffer::setDepth(int x, int y, uint16_t depth) {
    // assert(x >= 0);
    // assert(x < m_width);
    // assert(y >= 0);
    // assert(y < m_height);
    if (m_colorBuffer.empty() || x < 0 || x >= m_width || y < 0 || y >= m_height) {
        return;
    }
    m_depthBuffer[m_width * y + x] = depth;
}

uint16_t Framebuffer::getDepth(int x, int y) const {
    // assert(x >= 0);
    // assert(x < m_width);
    // assert(y >= 0);
    // assert(y < m_height);
    if (m_colorBuffer.empty() || x < 0 || x >= m_width || y < 0 || y >= m_height) {
        return 0;
    }
    return m_depthBuffer[m_width * y + x];
}

void Framebuffer::clearColor(const Colorf &color) {
    // TODO optimize
    std::fill(m_colorBuffer.begin(), m_colorBuffer.end(), color_f2i(color));
}

void Framebuffer::clearDepth(uint16_t depth) {
    // TODO optimize
    std::fill(m_depthBuffer.begin(), m_depthBuffer.end(), depth);
}

bool Framebuffer::saveColorBuffer(const std::string &file, bool flipVertically) const {
    std::vector<uint8_t> rgba(m_width * m_height * 4);
    pixels(rgba.data(), RGBA8888);
    stbi_flip_vertically_on_write(flipVertically);
    return stbi_write_png(file.c_str(), m_width, m_height, 4, rgba.data(), 0);
}

bool Framebuffer::saveDepthBuffer(const std::string &file, bool flipVertically) const {
    int nPixels = m_width * m_height;
    std::vector<uint8_t> gray(nPixels);
    for (int i = 0; i < nPixels; ++i) {
        gray[i] = (uint8_t) std::round(float(m_depthBuffer[i]) / 65535.0f * 255.0f);
    }
    stbi_flip_vertically_on_write(flipVertically);
    return stbi_write_png(file.c_str(), m_width, m_height, 1, gray.data(), 0);
}

void Framebuffer::pixels(uint8_t *pixels, int format) const {
    // TODO optimize
    int nPixels = m_width * m_height;
    switch (format) {
        case RGBA8888:
            for (int i = 0; i < nPixels; ++i) {
                pixels[i * 4 + 0] = m_colorBuffer[i].pixel.r;
                pixels[i * 4 + 1] = m_colorBuffer[i].pixel.g;
                pixels[i * 4 + 2] = m_colorBuffer[i].pixel.b;
                pixels[i * 4 + 3] = m_colorBuffer[i].pixel.a;
            }
            break;
        case BGRA8888:
            memcpy(pixels, m_colorBuffer.data(), nPixels * 4);
            break;
        default:
            break;
    }
}
