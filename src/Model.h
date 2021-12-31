//
// Created by xiaozhuai on 2021/12/29.
//

#ifndef SOFT_RENDERER_MODEL_H
#define SOFT_RENDERER_MODEL_H

#include "Texture2D.h"
#include <vector>
#include <array>
#include <glm/glm.hpp>

class Model {
public:
    explicit Model(const std::string &filename);

    inline int nVerts() const {
        return m_verts.size();
    }

    inline int nFaces() const {
        return m_vFaces.size();
    }

    inline const std::vector<int> &face(int faceIndex) const {
        return m_vFaces[faceIndex];
    }

    inline const glm::vec3 &vert(int faceIndex, int index) const {
        return m_verts[m_vFaces[faceIndex][index]];
    }

    inline const glm::vec3 &normal(int faceIndex, int index) const {
        return m_norms[m_vnFaces[faceIndex][index]];
    }

    inline const glm::vec2 &uv(int faceIndex, int index) const {
        return m_uvs[m_vtFaces[faceIndex][index]];
    }

    inline const Texture2D &diffuse() const {
        return m_diffuse;
    }

private:
    static void loadTexture(const std::string &filename, const std::string &suffix, Texture2D &texture);

private:
    std::vector<glm::vec3> m_verts;
    std::vector<glm::vec3> m_norms;
    std::vector<glm::vec2> m_uvs;
    std::vector<std::vector<int>> m_vFaces;
    std::vector<std::vector<int>> m_vtFaces;
    std::vector<std::vector<int>> m_vnFaces;
    Texture2D m_diffuse;

};


#endif //SOFT_RENDERER_MODEL_H
