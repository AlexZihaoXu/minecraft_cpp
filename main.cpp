#include "engine/engine.h"
#include <iostream>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>


using namespace std::string_literals;

class GameWindow : public engine::Window {
    using engine::Window::Window;

public:

    void onSetup() override {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        ImGui_ImplGlfw_InitForOpenGL(getHandle(), true);
        ImGui_ImplOpenGL3_Init();

        // font

    }

    void onRender(double dt) override {
        glEnable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        var content = "Centered";
        var size = 80;
        var font = engine::Font::get("res/arial.ttf");
        var text = "AWA";

        var proj = glm::perspective(glm::radians(90.0f), getWidth() / (float) getHeight(), 0.1f, 1000.0f);
        for (int i = -20; i < 20; ++i) {
            var obj = glm::mat4(1.0f);
            obj = glm::translate(obj, {0, 0, -size * 1.5});
            obj = glm::rotate(obj, (float) glfwGetTime(), {0, 1, 0});
            obj = glm::rotate(obj, sin((float) glfwGetTime() / 1.2f) * 0.3f, {1, 0, 0});
            obj = glm::rotate(obj, cos((float) glfwGetTime() / 0.8f) * 0.3f, {0, 0, 1});
            obj = glm::translate(obj, {-font->getWidth(text, size) / 2, -size / 2, -i / 4.0f});
            font->render(text, {1, 1, 1, 1}, size, proj * obj);
        }


        renderGUI(dt);
    }

    void renderGUI(double dt) {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::SetWindowSize("awa", {200, 80});
        ImGui::Begin("awa", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);
        ImGui::Text("FPS: %4d", getFPS());
        ImGui::Text("Delta: %3.2f ms", dt * 1000);
        ImGui::End();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void onKeyUpdate(int key, bool down) override {

    }

    void onScroll(double x, double y) override {
    }

    void onMouseButton(int button, bool down) override {

    }

    void onMouseMotion(double x, double y) override {

    }

    void onDestroy() override {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }
};

int main() {
    GameWindow("Hello world", 856, 482).run();
    return 0;
}
