//
// Created by Alex on 9/7/2022.
//

#ifndef MINECRAFT_CLIENT_H
#define MINECRAFT_CLIENT_H

#include "../common/common.h"
#include "../../engine/engine.h"

namespace minecraft {
    class MinecraftClientWindow : public engine::Window {
    private:
        inline static MinecraftClientWindow *instance = nullptr;

        MinecraftClientWindow() : engine::Window("Minecraft AE", 856, 482) {
            instance = this;
        }

    public:
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
            minecraft::blocks::registerBlocks();
        }

        void onRender(double dt) override {
            GLCall(glClearColor(0, 0, 0, 0));
            GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
            GLCall(glEnable(GL_BLEND));
            GLCall(glEnable(GL_DEPTH_TEST));
            GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));


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
                    "FPS: "s + std::to_string(getFPS())
            };

            for (int i = 0; i < texts.size(); ++i) {
                var text = texts[i];
                var tr = glm::translate(trans, {0, getHeight() - (font->getHeight(size) + 2) * (i + 1) - 2, 0});
                font->render(text, {1, 1, 1, 1}, size, tr);
            }
        }

    };
}

#endif //MINECRAFT_CLIENT_H
