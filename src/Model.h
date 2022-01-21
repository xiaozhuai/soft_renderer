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

    inline const Texture2D &specular() const {
        return m_specular;
    }

    inline void verts(std::vector<glm::vec3> &arr) const {
        int nf = nFaces();
        arr.resize(nf * 3);
        for (int faceIndex = 0, i = 0; faceIndex < nf; ++faceIndex) {
            arr[i++] = vert(faceIndex, 0);
            arr[i++] = vert(faceIndex, 1);
            arr[i++] = vert(faceIndex, 2);
        }
    }

    inline void uvs(std::vector<glm::vec2> &arr) const {
        int nf = nFaces();
        arr.resize(nf * 3);
        for (int faceIndex = 0, i = 0; faceIndex < nf; ++faceIndex) {
            arr[i++] = uv(faceIndex, 0);
            arr[i++] = uv(faceIndex, 1);
            arr[i++] = uv(faceIndex, 2);
        }
    }

    inline void normals(std::vector<glm::vec3> &arr) const {
        int nf = nFaces();
        arr.resize(nf * 3);
        for (int faceIndex = 0, i = 0; faceIndex < nf; ++faceIndex) {
            arr[i++] = normal(faceIndex, 0);
            arr[i++] = normal(faceIndex, 1);
            arr[i++] = normal(faceIndex, 2);
        }
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
    Texture2D m_specular;

};


#endif //SOFT_RENDERER_MODEL_H
