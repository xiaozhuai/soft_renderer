//
// Created by xiaozhuai on 2021/12/30.
//

#ifndef SOFT_RENDERER_TEXTURE2D_H
#define SOFT_RENDERER_TEXTURE2D_H

#include "Types.h"

#include <string>
#include <vector>

// TODO minFilter magFilter wrapS wrapT

class Texture2D {
public:
    Texture2D() = default;

    Texture2D(int width, int height, const uint8_t *data = nullptr);

    explicit Texture2D(const std::string &file, bool flipVertically = true);

    bool load(const std::string &file, bool flipVertically = true);

    inline bool valid() const { return !m_pixels.empty(); }

    Colorf texture(const glm::vec2 &uv) const;

private:
    int m_width = 0;
    int m_height = 0;
    std::vector<uint8_t> m_pixels; // RGBA8888

};


#endif //SOFT_RENDERER_TEXTURE2D_H
