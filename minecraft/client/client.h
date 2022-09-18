//
// Created by Alex on 9/7/2022.
//

#ifndef MINECRAFT_CLIENT_H
#define MINECRAFT_CLIENT_H

#include "../common/common.h"
#include "../../engine/engine.h"
#include "render/client_render.h"

namespace minecraft {
    namespace client {

        class WindowEventHandler {
        public:

            WindowEventHandler() {

            }

            virtual ~WindowEventHandler() {
            }

            virtual void onMouseMove(double x, double y) {

            }

            virtual void onMouseDown(int button) {

            }

            virtual void onMouseUp(int button) {

            }

            virtual void onKeydown(int key) {

            }

            virtual void onKeyup(int key) {

            }

            virtual void onRender(double dt) {

            }

            virtual void onFocusChange(bool focused) {

            }

        };

        class MinecraftClientWindow : public engine::Window {
        private:
            inline static MinecraftClientWindow *instance = nullptr;
            std::vector<WindowEventHandler *> eventHandlers;
            std::vector<WindowEventHandler *> autoFreeEventHandlers;
            engine::VertexArrayObject *postProcessingVAO;
            engine::VertexBufferObject *postProcessingVBO;
            engine::ShaderProgram *postProcessingShader;

            MinecraftClientWindow() : engine::Window("Minecraft AE", 856, 482) {
                instance = this;
                postProcessingVBO = new engine::VertexBufferObject({
                                                                           -1, -1, 0, 0,
                                                                           -1, 1, 0, 1,
                                                                           1, 1, 1, 1,

                                                                           -1, -1, 0, 0,
                                                                           1, 1, 1, 1,
                                                                           1, -1, 1, 0
                                                                   });
                postProcessingVAO = new engine::VertexArrayObject(postProcessingVBO, {2, 2});
                var vertShader = new engine::Shader(GL_VERTEX_SHADER, R"(
#version 330 core

layout (location=0) in vec2 aPos;
layout (location=1) in vec2 aTexCoords;
out vec2 texCoords;

void main() {
    texCoords = aTexCoords;
    gl_Position = vec4(aPos, 0.0, 1.0);
}
)");
                var fragShader = new engine::Shader(GL_FRAGMENT_SHADER, R"(
#version 330 core

in vec2 texCoords;
out vec4 FragColor;
uniform vec2 size;
uniform sampler2D tex0;
uniform float msaa;

float width = size[0];
float height = size[1];
float x = texCoords.x * width;
float y = texCoords.y * height;

vec3 getColor(float x, float y) {
    return texture(tex0, vec2(x / width, y / height)).rgb;
}

float getLuma(vec3 color) {
    return sqrt(0.299 * color.r * color.r + 0.587 * color.g * color.g + 0.144 * color.b * color.b);
}

void main() {
    float count = 0;
    vec3 colorSum = vec3(0);
    for (float nx = -msaa / 2; nx <= msaa / 2; nx++) {
        for (float ny = -msaa / 2; ny <= msaa / 2; ny++) {
            colorSum += getColor(x + nx, y + ny);
            count++;
        }
    }

    FragColor = vec4(colorSum / count, 1);
}
)");
                postProcessingShader = (new engine::ShaderProgram())->attachShader(vertShader).attachShader(
                        fragShader).link();
                delete vertShader;
                delete fragShader;
            }

        public:

            int MSAA_LEVEL = 3;

            void addEventHandler(WindowEventHandler *handler) {
                eventHandlers.push_back(handler);
            }

            void removeEventHandler(WindowEventHandler *handler) {
                std::remove(eventHandlers.begin(), eventHandlers.end(), handler);
            }

            void addEventHandlerAutoRemove(WindowEventHandler *handler) {
                addEventHandler(handler);
                autoFreeEventHandlers.push_back(handler);
            }

            static MinecraftClientWindow *getInstance() {
                if (instance == nullptr) {
                    instance = new MinecraftClientWindow();
                }
                return instance;
            }

            static void launch() {
                getInstance()->run();
            }

            static void cleanup() {
                delete instance;
            }

            blocks::ChunkSection* chunkSection;
            render::ChunkSectionRenderer* chunkSectionRenderer;
            void onSetup() override {
                minecraft::blocks::Blocks::registerBlocks();
                minecraft::client::render::BlockRenderer::initialize();
                chunkSection = new blocks::ChunkSection();
                chunkSectionRenderer = new render::ChunkSectionRenderer(chunkSection);

                for (int x = -8; x < 8; ++x) {
                    for (int y = -8; y < 8; ++y) {
                        for (int z = -8; z < 8; ++z) {
                            if (std::sqrt(x * x + y * y + z * z) < 7)
                                chunkSection->setBlock(blocks::Blocks::get()->GRASS_BLOCK, x + 8, y + 8, z + 8);
                        }
                    }
                }
            }

            render::Camera cam;
            engine::Framebuffer *framebuffer = nullptr;

            void onRender(double dt) override {
                chunkSectionRenderer->update();

                int width, height;
                glfwGetFramebufferSize(getHandle(), &width, &height);
                if (!framebuffer) {
                    framebuffer = new engine::Framebuffer(width * MSAA_LEVEL, height * MSAA_LEVEL);
                }
                if (framebuffer->width() != width * MSAA_LEVEL ||
                    framebuffer->height() != height * MSAA_LEVEL) {
                    delete framebuffer;
                    framebuffer = new engine::Framebuffer(width * MSAA_LEVEL, height * MSAA_LEVEL);
                }
                GLCall(glViewport(0, 0, framebuffer->width(), framebuffer->height()));

                framebuffer->unbindContext();
                framebuffer->bindContext();
                for (var handler: eventHandlers) {
                    handler->onRender(dt);
                }

                GLCall(glClearColor(0.3, 0.3, 0.3, 1));
                GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
                GLCall(glEnable(GL_BLEND));
                GLCall(glEnable(GL_DEPTH_TEST));
                GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

                {
                    var model = glm::rotate(glm::translate(glm::mat4(1), {0, 0, -10}), 0.0f, {0, 1, 0});
                    render::BlockRenderer::renderBlock(blocks::Blocks::get()->GRASS_BLOCK, cam.projMat() * model);
                }
                {
                    var model = glm::rotate(glm::translate(glm::mat4(1), {0, 0, -5}), 0.0f, {0, 1, 0});
                    chunkSectionRenderer->render(cam.projMat() * model);
                }


                GLCall(glDisable(GL_DEPTH_TEST));

                framebuffer->unbindContext();
                {

                    GLCall(glViewport(0, 0, width, height));
                    glActiveTexture(GL_TEXTURE0);
                    framebuffer->bind();
                    postProcessingShader->setInt("tex0", 0);
                    postProcessingShader->setFloat("msaa", MSAA_LEVEL);
                    postProcessingShader->setVec2("size", {framebuffer->width(), framebuffer->height()});
                    postProcessingShader->render(postProcessingVAO);
                }
                onRenderGUI(dt);
            }

            void onRenderGUI(double dt) {
                onRenderGUI_Debug(dt);
            }

            void onRenderGUI_Debug(double dt) {
                using namespace std::string_literals;
                var size = 16;
                var font = engine::Font::get("res/Minecraftia-Regular.ttf");
                var trans = glm::ortho(0.0f, (float) getWidth(), 0.0f, (float) getHeight());
                std::vector<std::string> texts = {
                        "Minecraft (Alex Edition) ["s + __DATE__ + " " + __TIME__ + " built]",
                        "GLFW Version: "s + glfwGetVersionString(),
                        "FPS: "s + std::to_string(getFPS()),
                        "Cam: "s + std::to_string((int) cam.position.x) + ", " + std::to_string((int) cam.position.y) +
                        ", " + std::to_string((int) cam.position.z)
                };

                for (int i = 0; i < texts.size(); ++i) {
                    var text = texts[i];
                    var tr = glm::translate(trans, {0, getHeight() - (font->getHeight(size) + 2) * (i + 1) - 2, 0});
                    font->render(text, {1, 1, 1, 1}, size, tr);
                }
            }

            void onKeyUpdate(int key, bool down) override {
                if (down) {
                    for (var handler: this->eventHandlers) {
                        handler->onKeydown(key);
                    }
                } else {
                    for (var handler: this->eventHandlers) {
                        handler->onKeyup(key);
                    }
                }
            }

            void onMouseButton(int button, bool down) override {
                if (down) {
                    for (var handler: this->eventHandlers) {
                        handler->onMouseDown(button);
                    }
                } else {
                    for (var handler: this->eventHandlers) {
                        handler->onMouseUp(button);
                    }
                }
            }

            void onMouseMotion(double x, double y) override {
                for (var handler: this->eventHandlers) {
                    handler->onMouseMove(x, y);
                }
            }

            void onDestroy() override {
                for (var handler: this->autoFreeEventHandlers)
                    delete handler;
            }

            void onFocusChange(bool focused) override {
                for (var handler: this->eventHandlers) {
                    handler->onFocusChange(focused);
                }
            }
        };
    }

}

#endif //MINECRAFT_CLIENT_H
