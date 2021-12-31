//
// Created by xiaozhuai on 2021/12/29.
//

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include "Model.h"


Model::Model(const std::string &filename) {
    std::ifstream in;
    in.open(filename, std::ifstream::in);
    if (in.fail()) {
        std::cerr << "Can't read file " << filename << "\n";
        return;
    }
    std::string line;
    while (!in.eof()) {
        std::getline(in, line);
        std::istringstream iss(line);
        char trash;
        if (!line.compare(0, 2, "v ")) {
            iss >> trash;
            glm::vec3 v;
            for (int i = 0; i < 3; i++) {
                iss >> v[i];
            }
            m_verts.emplace_back(v);
        } else if (!line.compare(0, 3, "vn ")) {
            iss >> trash >> trash;
            glm::vec3 vn;
            for (int i = 0; i < 3; i++) {
                iss >> vn[i];
            }
            m_norms.emplace_back(vn);
        } else if (!line.compare(0, 3, "vt ")) {
            iss >> trash >> trash;
            glm::vec2 vt;
            for (int i = 0; i < 2; i++) {
                iss >> vt[i];
            }
            m_uvs.emplace_back(vt);
        } else if (!line.compare(0, 2, "f ")) {
            iss >> trash;
            std::vector<int> vf;
            std::vector<int> vtf;
            std::vector<int> vnf;
            int v_idx, vt_idx, vn_idx;
            while (iss >> v_idx >> trash >> vt_idx >> trash >> vn_idx) {
                v_idx--;
                vt_idx--;
                vn_idx--;
                vf.push_back(v_idx);
                vtf.push_back(vt_idx);
                vnf.push_back(vn_idx);
            }
            m_vFaces.emplace_back(vf);
            m_vnFaces.emplace_back(vnf);
            m_vtFaces.emplace_back(vtf);
        }
    }

    loadTexture(filename, "_diffuse.tga", m_diffuse);
}

void Model::loadTexture(const std::string &filename, const std::string &suffix, Texture2D &texture) {
    auto f = filename.substr(0, filename.find_last_of('.')) + suffix;
    texture.load(f);
}
