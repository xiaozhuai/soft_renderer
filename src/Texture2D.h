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

    inline bool valid() const { return m_pixels != nullptr; }

    Colorf texture(const glm::vec2 &uv) const;

private:
    inline int size() const { return m_width * m_height * 4; }

    inline void resize() {
        int s = size();
        if (s == 0) m_pixels.reset();
        else m_pixels = std::shared_ptr<uint8_t>(new uint8_t[s], std::default_delete<uint8_t[]>());
    }

private:
    int m_width = 0;
    int m_height = 0;
    std::shared_ptr<uint8_t> m_pixels; // RGBA8888

};


#endif //SOFT_RENDERER_TEXTURE2D_H
