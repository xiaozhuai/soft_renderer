//
// Created by xiaozhuai on 2021/12/29.
//

#ifndef SOFT_RENDERER_FRAMEBUFFER_H
#define SOFT_RENDERER_FRAMEBUFFER_H

#include "Types.h"

#include <string>
#include <vector>
#include <array>

class Framebuffer {
public:
    enum {
        RGBA8888 = 0,
        BGRA8888,
    };

public:
    Framebuffer(int width, int height);

    void resize(int width, int height);

    void setColor(int x, int y, const Colorf &color);

    Colorf getColor(int x, int y) const;

    void setDepth(int x, int y, uint16_t depth);

    uint16_t getDepth(int x, int y) const;

    void clearColor(const Colorf &color = Colorf(0.0f, 0.0f, 0.0f, 0.0f));

    void clearDepth(uint16_t depth = std::numeric_limits<uint16_t>::max());

    inline void clear(
            const Colorf &color = Colorf(0.0f, 0.0f, 0.0f, 0.0f),
            uint16_t depth = std::numeric_limits<uint16_t>::max()) {
        clearColor(color);
        clearDepth(depth);
    }

    bool saveColorBuffer(const std::string &file, bool flipVertically = false) const;

    bool saveDepthBuffer(const std::string &file, bool flipVertically = false) const;

    inline int width() const { return m_width; }

    inline int height() const { return m_height; }

    inline int depth() const { return std::numeric_limits<uint16_t>::max(); }

    void pixels(uint8_t *pixels, int format) const;

private:
    int m_width = 0;
    int m_height = 0;
    std::vector<ColorBGRA8888> m_colorBuffer; // RGBA8888
    std::vector<uint16_t> m_depthBuffer;
};


#endif //SOFT_RENDERER_FRAMEBUFFER_H
