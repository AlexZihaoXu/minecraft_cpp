#include "engine/engine.h"
#include <iostream>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

class GameWindow : public engine::Window {
    using engine::Window::Window;

public:
    engine::ShaderProgram *program;
    engine::VertexBufferObject *vbo;
    engine::VertexArrayObject *vao;
    engine::ElementBufferObject *ebo;
    engine::Texture *texture;
    engine::Framebuffer *framebuffer;

    void onSetup() override {
        program = new engine::ShaderProgram();
        var vertShader = new engine::Shader(GL_VERTEX_SHADER, R"(
#version 330 core

layout (location=0) in vec3 aPos;
layout (location=1) in vec2 aTexCoord;

out vec2 texCoord;
uniform mat4 proj;
uniform mat4 obj;

void main() {
    texCoord = aTexCoord;
    gl_Position = proj * obj * vec4(aPos, 1.0);
}
)");
        var fragShader = new engine::Shader(GL_FRAGMENT_SHADER, R"(
#version 330 core

in vec2 texCoord;
out vec4 FragColor;

uniform sampler2D texture0;

void main() {
    FragColor = texture(texture0, texCoord);;
}
)");
        program->attachShader(vertShader).attachShader(fragShader);
        program->link();
        vbo = new engine::VertexBufferObject({
                                                     0, 0, 0, 0, 0,
                                                     0, 0, 1, 0, 1,
                                                     1, 0, 0, 1, 0,
                                                     1, 0, 1, 1, 1,
                                                     0, 1, 0, 0, 1,
                                                     0, 1, 1, 0, 1,
                                                     1, 1, 0, 0, 1,
                                                     1, 1, 1, 0, 1

                                             });
        ebo = new engine::ElementBufferObject({
                                                      0, 4, 6, 0, 6, 2
                                              });
        vao = new engine::VertexArrayObject(vbo, ebo, {3, 2});
        texture = engine::Texture::get("res/cobblestone.png");

        delete vertShader;
        delete fragShader;

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        ImGui_ImplGlfw_InitForOpenGL(getHandle(), true);
        ImGui_ImplOpenGL3_Init();
        ImGui::StyleColorsDark();

        framebuffer = new engine::Framebuffer(16, 16);
        framebuffer->bindContext();
        glClearColor(1, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        bind();

    }

    float angle = 0;
    float yaw = 0;
    float pitch = 0;
    int objCount = 0;
    glm::vec3 pos{0, 0, 0};
    glm::vec3 vel{0, 0, 0};

    void onRender(double dt) override {
        glEnable(GL_DEPTH_TEST);
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        using namespace std::string_literals;

        program->use();
        vao->bind();

        angle += (cos((float) glfwGetTime()) + 2) * dt + (cos((float) glfwGetTime() * 0.2)) * dt;

        glm::mat4 proj = glm::perspective(glm::radians(100.0f), (float) getWidth() / (float) getHeight(), 0.1f,
                                          1500.0f);
        proj = glm::rotate(proj, pitch, {1.0f, 0.0f, 0.0f});
        proj = glm::rotate(proj, yaw, {0.0f, 1.0f, 0.0f});
        proj = glm::translate(proj, pos);
        objCount = std::max(1, objCount);
        for (float x = -objCount / 2.0f; x < objCount / 2.0f; ++x) {
            for (int y = -objCount / 2.0f; y < objCount / 2.0f; ++y) {
                glm::mat4 obj = glm::mat4(1.0f);
                obj = glm::translate(obj, {x * 4, y * 4, -10.0f});
                obj = glm::rotate(obj, (float) (cos(glfwGetTime() * 1.3 / 4)) / 10.0f, {1.0f, 0.0f, 0.0f});
                obj = glm::rotate(obj, (float) (sin(glfwGetTime() * 1.1 / 4 + 10)) / 10.0f, {0.0f, 0.0f, 1.0f});
                obj = glm::rotate(obj, angle, {0.0f, 1.0f, 0.0f});
                program->setMat4("proj", proj);
                program->setMat4("obj", obj);
                framebuffer->bind();
                program->setInt("texture0", 0);
                program->render(vao);
            }
        }

        var speed = 240;
        dt = std::min(1.0, dt);
        if (glfwGetKey(getHandle(), GLFW_KEY_W) == GLFW_PRESS) {
            vel.x += cos(yaw + M_PI / 2) * dt * speed;
            vel.z += sin(yaw + M_PI / 2) * dt * speed;
        }
        if (glfwGetKey(getHandle(), GLFW_KEY_S) == GLFW_PRESS) {
            vel.x -= cos(yaw + M_PI / 2) * dt * speed;
            vel.z -= sin(yaw + M_PI / 2) * dt * speed;
        }
        if (glfwGetKey(getHandle(), GLFW_KEY_A) == GLFW_PRESS) {
            vel.x += cos(yaw) * dt * speed;
            vel.z += sin(yaw) * dt * speed;
        }
        if (glfwGetKey(getHandle(), GLFW_KEY_D) == GLFW_PRESS) {
            vel.x -= cos(yaw) * dt * speed;
            vel.z -= sin(yaw) * dt * speed;
        }
        if (glfwGetKey(getHandle(), GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
            vel.y += dt * speed;
        }
        if (glfwGetKey(getHandle(), GLFW_KEY_SPACE) == GLFW_PRESS) {
            vel.y -= dt * speed;
        }

        pos.x += vel.x * dt;
        pos.y += vel.y * dt;
        pos.z += vel.z * dt;

        vel.x -= vel.x * std::min(1.0, dt * 5);
        vel.y -= vel.y * std::min(1.0, dt * 5);
        vel.z -= vel.z * std::min(1.0, dt * 5);

        // ImGUI
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::SetWindowSize("awa", {200, 80});
        ImGui::Begin("awa", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);
        ImGui::Text("FPS: %4d", getFPS());
        ImGui::Text("Delta: %3.2f ms", dt * 1000);
        ImGui::Text("Pos: %5.1f %5.1f %5.1f", pos.x, pos.y, pos.z);
        ImGui::Text("Object count: %d", objCount * objCount);
        ImGui::End();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    }

    bool locked = false;

    void onKeyUpdate(int key, bool down) override {
        if (down) {
            if (key == GLFW_KEY_ESCAPE) {
                glfwSetInputMode(getHandle(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                if (locked) {
                    glfwSetCursorPos(getHandle(), getWidth() / 2, getHeight() / 2);
                }
                locked = false;
            }
        }
    }

    void onScroll(double x, double y) override {
        objCount += y;
    }

    void onMouseButton(int button, bool down) override {
        if (down) {
            if (button == 0) {
                glfwSetInputMode(getHandle(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                if (!locked) {
                    glfwSetCursorPos(getHandle(), 0, 0);
                }
                locked = true;
            }
        }
    }

    void onMouseMotion(double x, double y) override {
        using namespace std;
        using namespace std::string_literals;

        if (locked) {
            yaw += x * 0.0024f;
            pitch += y * 0.0024f;
            yaw = std::fmod(yaw, M_PI * 2);
            pitch = std::min((float) (M_PI / 2), std::max((float) (-M_PI / 2), pitch));

            glfwSetCursorPos(getHandle(), 0, 0);
        }
    }

    void onDestroy() override {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        delete vbo;
        delete vao;
        delete program;
    }
};

int main() {
    GameWindow("Hello world", 856, 482).run();
    return 0;
}
