//
// Created by Alex on 9/11/2022.
//

#ifndef MINECRAFT_CLIENT_2D_RENDERER_H
#define MINECRAFT_CLIENT_2D_RENDERER_H

namespace minecraft::client::render {
    class Renderer2D {
    private:
        inline static bool initialized = false;
        inline static engine::ShaderProgram *shader;


    public:

        static void initialize() {
            if (initialized) return;

            console.info("Renderer", "Initializing 2D Renderer ... ");
            shader = new engine::ShaderProgram();
            var vertShader = new engine::Shader(GL_VERTEX_SHADER, R"(
#version 330 core

layout (location=0) in vec2 aPos;
out vec2 texCoords;
uniform mat4 trans;

void main() {
    texCoords = aPos;
    gl_Position = trans * vec4(aPos, 0.0, 1.0);
}
)");
            var fragShader = new engine::Shader(GL_FRAGMENT_SHADER, R"(
#version 330 core

in vec2 texCoords;
uniform sampler2D tex0;
uniform int mode;
uniform vec3 color;
uniform vec4 rect;
out vec4 FragColor;

#define TEXTURE 0

void main() {

    FragColor = texture(tex0, texCoords);
}

)");

            shader->attachShader(vertShader).attachShader(fragShader).link();
            delete vertShader;
            delete fragShader;

            initialized = true;
        }

    };
}

#endif //MINECRAFT_CLIENT_2D_RENDERER_H
