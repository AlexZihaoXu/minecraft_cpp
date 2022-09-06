#include "engine/engine.h"
#include <iostream>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <ft2build.h>
#include FT_FREETYPE_H

using namespace std::string_literals;

struct Character {
    unsigned int textureID;
    glm::ivec2 size;
    glm::ivec2 bearing;
    GLint advance;
};

std::map<char, Character> characters;

class GameWindow : public engine::Window {
    using engine::Window::Window;

public:

    engine::ShaderProgram *program;
    engine::VertexArrayObject *vao;
    engine::VertexBufferObject *vbo;

    void onSetup() override {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        ImGui_ImplGlfw_InitForOpenGL(getHandle(), true);
        ImGui_ImplOpenGL3_Init();

        // font

        FT_Library ft;
        if (FT_Init_FreeType(&ft)) {
            throw std::runtime_error("Couldn't initiate FreeType library.");
        }

        FT_Face face;
        if (FT_New_Face(ft, "res/Minecraftia-Regular.ttf", 0, &face)) {
            throw std::runtime_error("Failed to load font");
        }

        FT_Set_Pixel_Sizes(face, 0, 48);
        if (FT_Load_Char(face, 'X', FT_LOAD_RENDER)) {
            throw std::runtime_error("Failed to load Glyph");
        }

        console.info("Name: "s + std::string(face->family_name));

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        for (GLubyte c = 0; c < 128; c++) {
            if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
                console.err("Failed to load Glyph: "s + std::to_string((int) c));
                continue;
            }
            GLuint texture;
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(
                    GL_TEXTURE_2D,
                    0,
                    GL_RED,
                    face->glyph->bitmap.width,
                    face->glyph->bitmap.rows,
                    0,
                    GL_RED,
                    GL_UNSIGNED_BYTE,
                    face->glyph->bitmap.buffer
            );
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            Character character = {
                    texture,
                    glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                    glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                    face->glyph->advance.x
            };
            characters[c] = character;
        }

        FT_Done_Face(face);
        FT_Done_FreeType(ft);

        // Shader

        var vertShader = new engine::Shader(GL_VERTEX_SHADER, R"(
#version 330 core
layout (location=0) in vec2 aPos;
out vec2 texCoords;
uniform mat4 proj;
uniform float scale;
uniform vec2 pos;

void main()
{
    gl_Position = proj * vec4(pos.x + aPos.x * scale, pos.y + aPos.y * scale, 0.0, 1.0);
    texCoords = aPos;
}
)");
        var fragShader = new engine::Shader(GL_FRAGMENT_SHADER, R"(
#version 330 core
in vec2 texCoords;
out vec4 FragColor;
uniform sampler2D texture0;

void main()
{
    FragColor = vec4(1.0) * 0.8 * texture(texture0, texCoords * vec2(1, -1)) + vec4(1.0) * 0.2;
}
)");
        program = (new engine::ShaderProgram())->attachShader(vertShader).attachShader(fragShader).link();

        vbo = new engine::VertexBufferObject({
                                                     0, 0, 0, 1, 1, 1,
                                                     0, 0, 1, 1, 1, 0
                                             });
        vao = new engine::VertexArrayObject(vbo, {2});
    }

    void onRender(double dt) override {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        var proj = glm::ortho(0.0f, (float) getWidth(), 0.0f, (float) getHeight());
        program->use();
        program->setMat4("proj", proj);
        program->setFloat("scale", 128);
        program->setVec2("pos", {getWidth() / 2, getHeight() / 2});
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, characters['A'].textureID);
        program->setInt("texture0", 0);
        program->render(vao);

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
