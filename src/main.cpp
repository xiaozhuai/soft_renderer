//
// Created by xiaozhuai on 2020/10/10.
//

#include "Model.h"
#include "Texture2D.h"
#include "Window.h"
#include "Log.h"
#include "IShader.h"

#include <iostream>
#include <cmath>
#include <glm/glm.hpp>

Model model("assets/african_head.obj");

class VShader : public IVertexShader {
DEFINE_ATTRIBUTES_BEGIN()
    DEFINE_ATTRIBUTE(aPosition, 3)
    DEFINE_ATTRIBUTE(aTexCoords, 2)
    DEFINE_ATTRIBUTE(aNormal, 3)
DEFINE_ATTRIBUTES_END()

DEFINE_VARYINGS_BEGIN()
    DEFINE_VARYING(vPosition, 3)
    DEFINE_VARYING(vTexCoords, 2)
    DEFINE_VARYING(vNormal, 3)
DEFINE_VARYINGS_END()



public:
    void main(glm::vec4 &position) override {
        VAR_ATTRIBUTE(aPosition, 3);
        VAR_ATTRIBUTE(aTexCoords, 2);
        VAR_ATTRIBUTE(aNormal, 3);
        VAR_VARYING(vPosition, 3);
        VAR_VARYING(vTexCoords, 2);
        VAR_VARYING(vNormal, 3);
        position = glm::vec4(aPosition, 1.0f);
        vPosition = aPosition;
        vTexCoords = aTexCoords;
        vNormal = aNormal;
    }
};

class FShader : public IFragmentShader {

DEFINE_VARYINGS_BEGIN()
    DEFINE_VARYING(vPosition, 3)
    DEFINE_VARYING(vTexCoords, 2)
    DEFINE_VARYING(vNormal, 3)
DEFINE_VARYINGS_END()

public:
    void main(Colorf &fragColor, bool &discard) override {
        VAR_VARYING(vPosition, 3);
        VAR_VARYING(vTexCoords, 2);
        VAR_VARYING(vNormal, 3);

        // fragColor = {1.0f, 1.0f, 1.0f, 1.0f};

        glm::vec3 color = model.diffuse().texture(vTexCoords);
        auto n = -glm::normalize(vNormal);
        float intensity = glm::dot(n, lightDir);
        discard = intensity < 0.0f;
        fragColor = {color * intensity, 1.0f};
    }

private:
    const glm::vec3 lightDir{0.0f, 0.0f, -1.0f};
};

int main() {
    std::vector<glm::vec3> verts;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals;
    model.verts(verts);
    model.uvs(uvs);
    model.normals(normals);

    Program<VShader, FShader> program;
    program.attributePointer("aPosition", 3, (float *) verts.data());
    program.attributePointer("aTexCoords", 2, (float *) uvs.data());
    program.attributePointer("aNormal", 3, (float *) normals.data());

    Window window("Soft Renderer", 512, 512, WF_RESIZABLE);

    window.setHighDpi(false);
    printf("Framebuffer: %d, %d\n", window.getFramebufferWidth(), window.getFramebufferHeight());

    window.onUpdate([&](Framebuffer &framebuffer) {

        LOGP_BEG("clear");
        framebuffer.clear(Colorf(0.0f, 0.0f, 0.0f, 1.0f));
        LOGP_END("clear");

        LOGP_BEG("draw");
        program.drawTriangles(framebuffer, verts.size());
        // drawModelLines(model, framebuffer);
        // drawModelTriangles(model, framebuffer);
        LOGP_END("draw");
    });
    window.catchSignals();
    window.exec();
    return 0;
}
