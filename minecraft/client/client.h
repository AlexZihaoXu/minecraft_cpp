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

            MinecraftClientWindow() : engine::Window("Minecraft AE", 856, 482) {
                instance = this;
            }

        public:

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

            void onSetup() override {
                minecraft::blocks::Blocks::registerBlocks();
                minecraft::client::render::BlockRenderer::initialize();
            }

            render::Camera cam;

            void onRender(double dt) override {
                for (var handler: eventHandlers) {
                    handler->onRender(dt);
                }

                GLCall(glClearColor(0.3, 0.3, 0.3, 1));
                GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
                GLCall(glEnable(GL_BLEND));
                GLCall(glEnable(GL_DEPTH_TEST));
                GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

                var model = glm::rotate(glm::translate(glm::mat4(1), {0, 0, -10}), 0.0f, {0, 1, 0});
                render::BlockRenderer::renderBlock(blocks::Blocks::get()->GRASS_BLOCK, cam.projMat() * model);

                GLCall(glDisable(GL_DEPTH_TEST));
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
