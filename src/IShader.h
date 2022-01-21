//
// Created by xiaozhuai on 2021/12/30.
//

#ifndef SOFT_RENDERER_ISHADER_H
#define SOFT_RENDERER_ISHADER_H

#include "Types.h"
#include "Primitive.h"
#include <cassert>
#include <unordered_map>
#include <functional>
#include <utility>
#include <oneapi/tbb.h>

#define DEFINE_ATTRIBUTES_BEGIN()                                                       \
public:                                                                                 \
    static std::unordered_map<std::string, int> getAttributes() {                       \
        return std::unordered_map<std::string, int>{                                    \

#define DEFINE_ATTRIBUTES_END()                                                         \
        };                                                                              \
    }

#define DEFINE_ATTRIBUTE(name, size)                                                    \
            {#name, (size)},


#define DEFINE_VARYINGS_BEGIN()                                                         \
public:                                                                                 \
    static std::unordered_map<std::string, int> getVaryings() {                         \
        return std::unordered_map<std::string, int>{                                    \

#define DEFINE_VARYINGS_END()                                                           \
        };                                                                              \
    }

#define DEFINE_VARYING(name, size)                                                      \
            {#name, (size)},

#define VAR_ATTRIBUTE(name, size) auto &name = attribute<(size)>(#name)

#define VAR_VARYING(name, size) auto &name = varying<(size)>(#name)

class IVertexShader {
public:
    IVertexShader() = default;

    virtual ~IVertexShader() {}

    virtual void main(glm::vec4 &position) = 0;

    inline void setContext(int idx,
                           const std::unordered_map<std::string, AVData> *attributes,
                           std::unordered_map<std::string, AVData> *varyings) {
        m_idx = idx;
        m_attributes = attributes;
        m_varyings = varyings;
    }

protected:
    template<int size>
    inline const glm::vec<size, float, glm::defaultp> &attribute(const std::string &name) const {
        const auto &attr = (*m_attributes).at(name);
        assert(size >= 1);
        assert(size <= 4);
        assert(attr.size == size);
        return *(glm::vec<size, float, glm::defaultp> *) (attr.data.get() + (m_idx * attr.size));
    }

    template<int size>
    inline glm::vec<size, float, glm::defaultp> &varying(const std::string &name) const {
        auto &varying = (*m_varyings)[name];
        assert(size >= 1);
        assert(size <= 4);
        assert(varying.size == size);
        return *(glm::vec<size, float, glm::defaultp> *) (varying.data.get() + (m_idx * varying.size));
    }

private:
    int m_idx = -1;
    const std::unordered_map<std::string, AVData> *m_attributes = nullptr;
    std::unordered_map<std::string, AVData> *m_varyings = nullptr;
};

class IFragmentShader {
public:
    IFragmentShader() = default;

    virtual ~IFragmentShader() {}

    virtual void main(Colorf &fragColor, bool &discard) = 0;

    inline void setContext(const std::unordered_map<std::string, AVData> *varyings,
                           const std::unordered_map<std::string, glm::vec4> *varyingsValue) {
        m_varyings = varyings;
        m_varyingsValue = varyingsValue;
    }

protected:
    template<int size>
    inline const glm::vec<size, float, glm::defaultp> &varying(const std::string &name) const {
        auto &varying = (*m_varyings).at(name);
        assert(size >= 1);
        assert(size <= 4);
        assert(varying.size == size);
        return *((const glm::vec<size, float, glm::defaultp> *) (&(*m_varyingsValue).at(name)));
    }

private:
    const std::unordered_map<std::string, AVData> *m_varyings = nullptr;
    const std::unordered_map<std::string, glm::vec4> *m_varyingsValue;
};

template<typename VS, typename FS>
class Program final {
public:
    Program() = default;

    VS &getVertexShader() { return m_vsTpl; }

    FS &getFragmentShader() { return m_fsTpl; }

    void attributePointer(const std::string &name, int size, const float *data) {
        m_externalAttributes[name] = AttributeInfo{
                .size = size,
                .data = data
        };
    }

    void drawTriangles(Framebuffer &framebuffer, int count, int offset = 0) {
        alloc(count, offset);
        oneapi::tbb::parallel_for(0, count, [&](int i) {
            VS vs = m_vsTpl;
            vs.setContext(i, &m_attributes, &m_varyings);
            vs.main(m_positions[i]);
        });

        primitiveTriangles(
                framebuffer.width(), framebuffer.height(), framebuffer.depth(),
                m_positions,
                [this, &framebuffer](int offset, const std::array<glm::vec4i, 3> &pts) {
                    rasterizationTriangle(
                            framebuffer.width(), framebuffer.height(),
                            offset,
                            pts, m_varyings,
                            [this, &framebuffer](
                                    const glm::vec4i &p,
                                    const std::unordered_map<std::string, glm::vec4> &varyingsValue) {
                                if (p.z < std::numeric_limits<uint16_t>::min()
                                    || p.z > std::numeric_limits<uint16_t>::max()
                                    || p.z > framebuffer.getDepth(p.x, p.y)) {
                                    return;
                                }

                                FS fs = m_fsTpl;
                                fs.setContext(&m_varyings, &varyingsValue);

                                Colorf fragColor = {0.0f, 0.0f, 0.0f, 0.0f};
                                bool discard = false;
                                fs.main(fragColor, discard);
                                if (!discard) {
                                    framebuffer.setDepth(p.x, p.y, p.z);
                                    framebuffer.setColor(p.x, p.y, fragColor);
                                }
                            }
                    );
                }
        );
    }

    void alloc(int count, int offset = 0) {
        m_positions.resize(count);

        auto attrs = VS::getAttributes();
        for (const auto &it: attrs) {
            const std::string &name = it.first;
            int size = it.second;
            assert(size >= 1);
            assert(size <= 4);
            if (m_attributes.find(name) == m_attributes.end()) {
                m_attributes[name] = AVData{};
                m_attributes[name].alloc(name, size, count);
            } else if (size != m_attributes[name].size || count != m_attributes[name].count) {
                m_attributes[name].alloc(name, size, count);
            }
            if (m_externalAttributes.find(name) != m_externalAttributes.end()) {
                assert(m_attributes[name].size == m_externalAttributes[name].size);
                memcpy(m_attributes[name].data.get(),
                       m_externalAttributes[name].data + offset,
                       m_attributes[name].nBytes());
            }
        }

        auto varyings1 = VS::getVaryings();
        auto varyings2 = FS::getVaryings();
        std::unordered_map<std::string, int> varyings;
        for (const auto &it: varyings1) {
            const std::string &name = it.first;
            int size = it.second;
            if (varyings2.find(name) != varyings2.end()) {
                assert(varyings2[name] == size);
            }
            varyings[name] = size;
        }
        for (const auto &it: varyings2) {
            const std::string &name = it.first;
            int size = it.second;
            if (varyings1.find(name) != varyings1.end()) {
                assert(varyings1[name] == size);
            }
            varyings[name] = size;
        }

        for (const auto &it: varyings) {
            const std::string &name = it.first;
            int size = it.second;
            if (m_varyings.find(name) == m_varyings.end()) {
                m_varyings[name] = AVData{};
                m_varyings[name].alloc(name, size, count);
            } else if (size != m_varyings[name].size || count != m_varyings[name].count) {
                m_varyings[name].alloc(name, size, count);
            }
        }
    }

private:
    std::vector<glm::vec4> m_positions;
    std::unordered_map<std::string, AttributeInfo> m_externalAttributes;
    std::unordered_map<std::string, AVData> m_attributes;
    std::unordered_map<std::string, AVData> m_varyings;
    VS m_vsTpl;
    FS m_fsTpl;
};

#endif //SOFT_RENDERER_ISHADER_H
