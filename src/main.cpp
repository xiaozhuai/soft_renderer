//
// Created by xiaozhuai on 2020/10/10.
//

#include "Model.h"
#include "Texture2D.h"
#include "Window.h"
#include "Log.h"
#include "IShader.h"
#include "FppCamera.h"

#include <cmath>
#include <glm/glm.hpp>

class AfricanVShader : public IVertexShader {
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
        vPosition = glm::vec3(model * glm::vec4(aPosition, 1.0));
        vTexCoords = aTexCoords;
        vNormal = glm::mat3(glm::transpose(glm::inverse(model))) * aNormal;
        position = projection * view * model * glm::vec4(vPosition, 1.0f);
        // position = glm::vec4(aPosition, 1.0f);
        // vPosition = aPosition;
        // vTexCoords = aTexCoords;
        // vNormal = aNormal;
    }

    glm::mat4 projection = glm::mat4(1.0f);
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 model = glm::mat4(1.0f);
};

class AfricanFShader : public IFragmentShader {

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

        auto normal = glm::normalize(vNormal);
        auto lightDir = glm::normalize(lightPos - vPosition);
        auto viewDir = glm::normalize(viewPos - vPosition);
        auto reflectDir = glm::reflect(-lightDir, normal);
        glm::vec3 diffuseColor = diffuseTexture.texture(vTexCoords);
        glm::vec3 specularColor = specularTexture.texture(vTexCoords);

        // ambient lighting
        auto ambient = ambientColor * ambientStrength * diffuseColor;

        // diffuse lighting
        float diff = std::max(glm::dot(normal, lightDir), 0.0f);
        auto diffuse = lightColor * diffuseStrength * diff * diffuseColor;

        // specular lighting
        float spec = std::pow(std::max(glm::dot(viewDir, reflectDir), 0.0f), shininess);
        auto specular = lightColor * specularStrength * spec * specularColor;

        glm::vec3 color = ambient + diffuse + specular;
        fragColor = {color, 1.0f};
    }

    glm::vec3 viewPos{};
    Texture2D diffuseTexture;
    Texture2D specularTexture;

private:
    const glm::vec3 ambientColor{1.0f, 1.0f, 1.0f};
    const glm::vec3 lightColor{1.0f, 1.0f, 1.0f};

    const float ambientStrength = 0.1f;
    const glm::vec3 lightPos{0.5f, 1.0f, 2.0f};
    const float diffuseStrength = 0.8f;
    const float specularStrength = 0.5f;
    const float shininess = 32.0f;
};


class TriVShader : public IVertexShader {
DEFINE_ATTRIBUTES_BEGIN()
                                DEFINE_ATTRIBUTE(aPosition, 2)
                                DEFINE_ATTRIBUTE(aColor, 3)
    DEFINE_ATTRIBUTES_END()

DEFINE_VARYINGS_BEGIN()
                                DEFINE_VARYING(vColor, 3)
    DEFINE_VARYINGS_END()


public:
    void main(glm::vec4 &position) override {
        VAR_ATTRIBUTE(aPosition, 2);
        VAR_ATTRIBUTE(aColor, 3);
        VAR_VARYING(vColor, 3);
        position = glm::vec4(aPosition, 0, 1);
        vColor = aColor;
    }
};

class TriFShader : public IFragmentShader {

DEFINE_VARYINGS_BEGIN()
                                DEFINE_VARYING(vColor, 3)
    DEFINE_VARYINGS_END()

public:
    void main(Colorf &fragColor, bool &discard) override {
        VAR_VARYING(vColor, 3);
        fragColor = {vColor, 1.0f};
    }
};

int main() {
    Model headModel("assets/african_head.obj");
    std::vector<glm::vec3> headVerts;
    std::vector<glm::vec2> headUvs;
    std::vector<glm::vec3> headNormals;
    headModel.verts(headVerts);
    headModel.uvs(headUvs);
    headModel.normals(headNormals);

    Model headEyeInnerModel("assets/african_head_eye_inner.obj");
    std::vector<glm::vec3> headEyeInnerVerts;
    std::vector<glm::vec2> headEyeInnerUvs;
    std::vector<glm::vec3> headEyeInnerNormals;
    headEyeInnerModel.verts(headEyeInnerVerts);
    headEyeInnerModel.uvs(headEyeInnerUvs);
    headEyeInnerModel.normals(headEyeInnerNormals);

    FppCamera camera;
    camera.setFov(60.0f);
    camera.setPosition(glm::vec3(0.0f, 0.0f, 3.0f));
    camera.setPitch(0.0f);
    camera.setYaw(-90.0f);

    Program<AfricanVShader, AfricanFShader> africanProgram;
    Program<TriVShader, TriFShader> triProgram;

    Window window("Soft Renderer", 512, 512, WF_RESIZABLE);
    LOG("Framebuffer: %d, %d", window.getFramebufferWidth(), window.getFramebufferHeight());

    bool rotating = false;
    window.onMouseButton([&](mfb_mouse_button button, mfb_key_mod mod, bool isPressed) {
        if (button == MOUSE_BTN_2) {
            rotating = isPressed;
            if (rotating) {
                camera.resetMouseMove(window.getMouseX(), window.getMouseY());
            }
        }
    });
    window.onMouseMove([&](int x, int y) {
        if (rotating) camera.processMouseMove(x, y);
    });

    window.onMouseScroll([&](mfb_key_mod mod, float deltaX, float deltaY) {
        camera.processMouseScroll(deltaX, deltaY);
    });

    window.onKeyboard([&](mfb_key key, mfb_key_mod mod, bool isPressed) {
        // printf("%f %f %f ---> %f %f %f\n",
        //        camera.getPosition()[0], camera.getPosition()[1], camera.getPosition()[2],
        //        camera.getFront()[0], camera.getFront()[1], camera.getFront()[2]);
        if (key == KB_KEY_W || key == KB_KEY_UP) {
            camera.processMove(FppCamera::MoveDirection::Forward, 20);
        }
        if (key == KB_KEY_S || key == KB_KEY_DOWN) {
            camera.processMove(FppCamera::MoveDirection::Backward, 20);
        }
        if (key == KB_KEY_A || key == KB_KEY_LEFT) {
            camera.processMove(FppCamera::MoveDirection::Left, 20);
        }
        if (key == KB_KEY_D || key == KB_KEY_RIGHT) {
            camera.processMove(FppCamera::MoveDirection::Right, 20);
        }
        if (key == KB_KEY_SPACE) {
            camera.processMove(FppCamera::MoveDirection::Up, 20);
        }
        if (key == KB_KEY_Z) {
            camera.processMove(FppCamera::MoveDirection::Down, 20);
        }
    });

    float triVertex[] = {
            0.0f, 1.0f,
            -1.0f, -1.0f,
            1.0f, -1.0f
    };
    float triColor[] = {
            1.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 1.0f,
    };

    window.onUpdate([&](Framebuffer &framebuffer) {
        camera.setAspect((float) framebuffer.width() / (float) framebuffer.height());

        printf("%f %f %f ---> %f %f %f\n",
               camera.getPosition()[0], camera.getPosition()[1], camera.getPosition()[2],
               camera.getFront()[0], camera.getFront()[1], camera.getFront()[2]);

        LOGP_BEG("clear");
        framebuffer.clear(Colorf(0.0f, 0.0f, 0.0f, 1.0f));
        LOGP_END("clear");

        LOGP_BEG("draw");

        glm::mat4 model = glm::mat4(1.0f);

        africanProgram.attributePointer("aPosition", 3, (float *) headVerts.data());
        africanProgram.attributePointer("aTexCoords", 2, (float *) headUvs.data());
        africanProgram.attributePointer("aNormal", 3, (float *) headNormals.data());
        africanProgram.getVertexShader().projection = camera.getProjection();
        africanProgram.getVertexShader().view = camera.getView();
        africanProgram.getVertexShader().model = model;
        africanProgram.getFragmentShader().diffuseTexture = headModel.diffuse();
        africanProgram.getFragmentShader().specularTexture = headModel.specular();
        africanProgram.getFragmentShader().viewPos = camera.getPosition();
        africanProgram.drawTriangles(framebuffer, headVerts.size());

        africanProgram.attributePointer("aPosition", 3, (float *) headEyeInnerVerts.data());
        africanProgram.attributePointer("aTexCoords", 2, (float *) headEyeInnerUvs.data());
        africanProgram.attributePointer("aNormal", 3, (float *) headEyeInnerNormals.data());
        africanProgram.getVertexShader().projection = camera.getProjection();
        africanProgram.getVertexShader().view = camera.getView();
        africanProgram.getVertexShader().model = model;
        africanProgram.getFragmentShader().diffuseTexture = headEyeInnerModel.diffuse();
        africanProgram.getFragmentShader().specularTexture = headEyeInnerModel.specular();
        africanProgram.getFragmentShader().viewPos = camera.getPosition();
        africanProgram.drawTriangles(framebuffer, headEyeInnerVerts.size());

        // triProgram.attributePointer("aPosition", 2, triVertex);
        // triProgram.attributePointer("aColor", 3, triColor);
        // triProgram.drawTriangles(framebuffer, 3);

        LOGP_END("draw");
    });
    window.catchSignals();
    window.exec();
    return 0;
}
