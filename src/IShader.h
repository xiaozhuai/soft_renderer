//
// Created by xiaozhuai on 2021/12/30.
//

#ifndef SOFT_RENDERER_ISHADER_H
#define SOFT_RENDERER_ISHADER_H

#include "Types.h"

class IShader {
public:
    IShader() = default;

    virtual ~IShader() = default;;

    virtual glm::vec3 vertex(int faceIndex, int index) = 0;

    virtual bool fragment(Colorf &color) = 0;
};


#endif //SOFT_RENDERER_ISHADER_H
