//
// Created by Alex on 9/11/2022.
//

#ifndef MINECRAFT_CLIENT_2D_RENDERER_H
#define MINECRAFT_CLIENT_2D_RENDERER_H

namespace minecraft::client::render {

    struct Renderer2DState {
        glm::mat4 transform;
        glm::vec4 color;
    };

    class Renderer2D {
    private:
        inline static bool initialized = false;
        inline static engine::ShaderProgram *shader;
        inline static engine::VertexArrayObject *vao;
        glm::mat4 transform{};
        glm::vec4 color{1, 1, 1, 1};
        std::stack<Renderer2DState> statesStack;

        bool transformChanged = false;

        void applyTransform() {
            if (transformChanged) {
                shader->use();
                shader->setMat4("trans", transform);
                shader->setVec4("color", color);
                transformChanged = false;
            }
        }

        inline static int MODE_TEXTURE = 0;
        inline static int MODE_RECT = 1;

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
uniform vec4 rect;

void main() {
    texCoords = aPos;
    gl_Position = trans * vec4(rect[0] + aPos.x * rect[2], rect[1] + aPos.y * rect[3], 0.0, 1.0);
}
)");
            var fragShader = new engine::Shader(GL_FRAGMENT_SHADER, R"(
#version 330 core

in vec2 texCoords;
out vec4 FragColor;
uniform int mode;
uniform sampler2D tex0;
uniform vec4 color;

#define MODE_TEXTURE 0
#define MODE_RECT 1

void main() {
    if (mode == MODE_RECT) {
        FragColor = color;
    } else {
        FragColor = texture(tex0, texCoords) * color;
    }
}

)");

            shader->attachShader(vertShader).attachShader(fragShader).link();
            delete vertShader;
            delete fragShader;

            var vbo = new engine::VertexBufferObject({
                                                             0, 0, 0, 1, 1, 1,
                                                             0, 0, 1, 1, 1, 0
                                                     });

            vao = new engine::VertexArrayObject(vbo, {2});

            initialized = true;
        }

        Renderer2D() {
            initialize();
        }

        // Setters

        Renderer2D *setColor(float r, float g, float b, float a) {
            color.r = r;
            color.g = g;
            color.b = b;
            color.a = a;
            transformChanged = true;

            return this;
        }

        Renderer2D *setColor(float r, float g, float b) {
            return setColor(r, g, b, 1);
        }

        Renderer2D *setColor(float brightness, float alpha) {
            return setColor(brightness, brightness, brightness, alpha);
        }

        Renderer2D *setColor(float brightness) {
            return setColor(brightness, 1);
        }

        // Transformations

        Renderer2D *pushState() {
            statesStack.push({transform, color});
            return this;
        }

        Renderer2D *popState() {
            color = statesStack.top().color;
            transform = statesStack.top().transform;
            statesStack.pop();
            return this;
        }

        Renderer2D *resetTransform(float width, float height) {
            transformChanged = true;
            transform = glm::ortho(0.0f, (float) width, (float) height, 0.0f);
            return this;
        }

        Renderer2D *resetTransform(int width, int height) {
            return resetTransform((float) width, (float) height);
        }

        Renderer2D *translate(float x, float y) {
            transformChanged = true;
            transform = glm::translate(transform, {x, y, 0});
            return this;
        }

        Renderer2D *translate(const glm::vec2 &xy) {
            return translate(xy.x, xy.y);
        }

        Renderer2D *rotate(float angle) {
            transformChanged = true;
            transform = glm::rotate(transform, angle, {0, 0, 1});
            return this;
        }

        Renderer2D *scale(float x, float y) {
            transformChanged = true;
            transform = glm::scale(transform, {x, y, 1});
            return this;
        }

        Renderer2D *scale(float s) {
            return scale(s, s);
        }

        // Render

        Renderer2D *rect(float x, float y, float w, float h) {
            applyTransform();
            shader->use();
            shader->setInt("mode", MODE_RECT);
            shader->setVec4("rect", {x, y, w, h});

            shader->render(vao);

            return this;
        }

        Renderer2D *image(engine::Texture *texture, float x, float y) {
            applyTransform();
            shader->use();
            shader->setInt("mode", MODE_TEXTURE);
            shader->setInt("tex0", 0);
            shader->setVec4("rect", {x, y, texture->width(), texture->height()});
            texture->bind();

            shader->render(vao);

            return this;
        }

        Renderer2D *image(engine::Texture *texture, int x, int y) {
            return image(texture, (float) x, (float) y);
        }

        Renderer2D *image(engine::Framebuffer *framebuffer, float x, float y) {
            applyTransform();
            shader->use();
            shader->setInt("mode", MODE_TEXTURE);
            shader->setInt("tex0", 0);
            shader->setVec4("rect", {x, y, framebuffer->width(), framebuffer->height()});
            framebuffer->bind();

            shader->render(vao);

            return this;
        }

        Renderer2D *image(engine::Framebuffer *framebuffer, int x, int y) {
            return image(framebuffer, (float) x, (float) y);
        }
    };


}

#endif //MINECRAFT_CLIENT_2D_RENDERER_H
