//
// Created by xiaozhuai on 2020/10/10.
//

#include "Primitive.h"
#include "Model.h"
#include "Texture2D.h"
#include "Window.h"
#include "Log.h"
#include "Signals.h"
#include "IShader.h"

#include <iostream>
#include <cmath>
#include <glm/glm.hpp>

#define COLOR_BLACK     Colorf(0.0f, 0.0f, 0.0f, 1.0f)
#define COLOR_RED       Colorf(1.0f, 0.0f, 0.0f, 1.0f)
#define COLOR_GREEN     Colorf(0.0f, 1.0f, 0.0f, 1.0f)
#define COLOR_BLUE      Colorf(0.0f, 0.0f, 1.0f, 1.0f)
#define COLOR_WHITE     Colorf(1.0f, 1.0f, 1.0f, 1.0f)

void drawModelLines(const Model &model, Framebuffer &framebuffer) {
    for (int faceIndex = 0; faceIndex < model.nFaces(); ++faceIndex) {
        for (int index = 0; index < 3; ++index) {
            std::array<glm::vec3, 2> pts = {model.vert(faceIndex, index), model.vert(faceIndex, (index + 1) % 3)};
            drawLine(framebuffer, pts, COLOR_RED);
        }
    }
}

void drawModelTriangles(const Model &model, Framebuffer &framebuffer) {
    const glm::vec3 lightDir(0.0f, 0.0f, -1.0f);
    for (int faceIndex = 0; faceIndex < model.nFaces(); ++faceIndex) {
        std::array<glm::vec3, 3> pts = {
                model.vert(faceIndex, 0),
                model.vert(faceIndex, 1),
                model.vert(faceIndex, 2)
        };
        std::array<glm::vec2, 3> pts2 = {
                model.uv(faceIndex, 0),
                model.uv(faceIndex, 1),
                model.uv(faceIndex, 2)
        };
        std::array<glm::vec3, 3> pts3 = {
                model.normal(faceIndex, 0),
                model.normal(faceIndex, 1),
                model.normal(faceIndex, 2)
        };
        drawTriangle(framebuffer, pts, pts2, pts3, lightDir, model.diffuse());
    }
    // framebuffer.saveDepthBuffer("depth.png");
}

int main() {
    Model model("assets/african_head.obj");

    Window window("Soft Renderer", 512, 512, WF_RESIZABLE);

#if defined(_WIN32)
    auto catchSignals = {SIGINT, SIGTERM};
#else
    auto catchSignals = {SIGINT, SIGTERM, SIGHUP, SIGQUIT};
#endif
    Signals::catchSome(catchSignals, [&]() {
        window.close();
    });

    window.setHighDpi(false);
    printf("Framebuffer: %d, %d\n", window.getFramebufferWidth(), window.getFramebufferHeight());

    window.onUpdate([&](Framebuffer &framebuffer) {
        LOGP_BEG("clear");
        framebuffer.clear(COLOR_BLACK);
        LOGP_END("clear");

        LOGP_BEG("draw");
        // drawModelLines(model, framebuffer);
        drawModelTriangles(model, framebuffer);
        LOGP_END("draw");
    });
    window.exec();
    return 0;
}
