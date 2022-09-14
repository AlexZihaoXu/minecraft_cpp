//
// Created by Alex on 9/11/2022.
//

#ifndef MINECRAFT_CLIENT_BLOCK_RENDERER_H
#define MINECRAFT_CLIENT_BLOCK_RENDERER_H

namespace minecraft::client::render {
    class BlockRenderer {
    private:
        inline static bool initialized = false;
        inline static engine::Framebuffer *textureAtlasBlocks;
        inline static std::map<std::string, glm::vec4> textureAtlasRangeTable;
        inline static engine::ShaderProgram *program;
        inline static std::map<int, engine::VertexArrayObject *> blockModels;

        static float map(float x, float begin, float end) {
            return begin + x * (end - begin);
        }

        static void initShaders() {
            var vertShader = new engine::Shader(GL_VERTEX_SHADER, R"(
#version 330 core
layout (location=0) in vec3 aPos;
layout (location=1) in vec2 aTexCoords;

out vec2 texCoords;

uniform mat4 trans;

void main() {
    texCoords = aTexCoords;
    gl_Position = trans * vec4(aPos, 1.0);
}
)");
            var fragShader = new engine::Shader(GL_FRAGMENT_SHADER, R"(
#version 330 core

in vec2 texCoords;
out vec4 FragColor;

uniform sampler2D tex0;

void main() {
    FragColor = texture(tex0, texCoords);
}
)");

            program = new engine::ShaderProgram();
            program->attachShader(vertShader).attachShader(fragShader).link();


            delete vertShader;
            delete fragShader;
        }

        static engine::VertexArrayObject *getBlockVAO(blocks::Block *&block) {
            if (!blockModels.contains(block->getID())) {
                std::vector<GLfloat> vertices;
                for (var def: block->getModelFaceRenderDefinition()) {
                    var texRange = textureAtlasRangeTable[def.texturePath];

                    vertices.push_back(def.point1.modelCoords.x);
                    vertices.push_back(def.point1.modelCoords.y);
                    vertices.push_back(def.point1.modelCoords.z);
                    vertices.push_back(map(def.point1.texCoords.x, texRange[0], texRange[2]));
                    vertices.push_back(map(def.point1.texCoords.y, texRange[1], texRange[3]));

                    vertices.push_back(def.point2.modelCoords.x);
                    vertices.push_back(def.point2.modelCoords.y);
                    vertices.push_back(def.point2.modelCoords.z);
                    vertices.push_back(map(def.point2.texCoords.x, texRange[0], texRange[2]));
                    vertices.push_back(map(def.point2.texCoords.y, texRange[1], texRange[3]));

                    vertices.push_back(def.point3.modelCoords.x);
                    vertices.push_back(def.point3.modelCoords.y);
                    vertices.push_back(def.point3.modelCoords.z);
                    vertices.push_back(map(def.point3.texCoords.x, texRange[0], texRange[2]));
                    vertices.push_back(map(def.point3.texCoords.y, texRange[1], texRange[3]));

                }
                blockModels[block->getID()] = new engine::VertexArrayObject(new engine::VertexBufferObject(vertices),
                                                                            {3, 2});
            }
            return blockModels[block->getID()];
        }

    public:

        static engine::Framebuffer *getTextureAtlasBlocks() {
            return textureAtlasBlocks;
        }

        static void initialize() {
            if (initialized)
                return;
            console.info("Initializing block renderer ...");
            std::vector<std::string> texPaths;
            std::set<std::string> texPathsSet;

            for (int i = 0; blocks::Block::get(i) != nullptr; ++i) {
                var block = blocks::Block::get(i);
                console.info("Renderer", "Loading block: " + block->getName());
                for (var def: block->getModelFaceRenderDefinition()) {
                    if (texPathsSet.contains(def.texturePath))
                        continue;
                    texPathsSet.insert(def.texturePath);
                    texPaths.push_back(def.texturePath);
                }
            }

            int totalWidth = 0;
            int totalHeight = 0;
            for (var path: texPaths) {
                var texture = engine::Texture::get(path);
                totalWidth += texture->width();
                totalHeight = std::max(totalHeight, texture->height());
            }

            textureAtlasBlocks = new engine::Framebuffer(totalWidth, totalHeight);
            textureAtlasBlocks->bindContext();
            GLCall(glViewport(0, 0, textureAtlasBlocks->width(), textureAtlasBlocks->height()));
            Renderer2D renderer;
            renderer.resetTransform(textureAtlasBlocks->width(), textureAtlasBlocks->height());
            int offset = 0;
            for (var texPath: texPaths) {
                var tex = engine::Texture::get(texPath);
                renderer.pushState()
                        ->translate(0, (float) tex->height())
                        ->scale(1, -1)
                        ->image(tex, offset, 0)
                        ->popState();
                textureAtlasRangeTable[texPath] = {
                        (float) offset / (float) totalWidth, (float) tex->height() / (float) totalHeight, ((float) offset + (float) tex->width()) / (float) totalWidth, 0
                };
                std::cout << texPath << ": " << offset / totalWidth << ' ' << 0 << ' '
                          << (offset + tex->width()) / totalWidth << ' ' << tex->height() / totalHeight << std::endl;
                offset += tex->width();
            }
            textureAtlasBlocks->unbindContext();
            console.info(
                    "Renderer",
                    "Created block texture atlas (" + std::to_string(totalWidth) + "x" + std::to_string(totalHeight) +
                    ")");
            initShaders();
            initialized = true;
        }

        static void renderBlock(blocks::Block *block, glm::mat4 trans) {
            initialize();

            var model = getBlockVAO(block);

            program->use();
            program->setMat4("trans", trans);
            textureAtlasBlocks->bind();
            program->setInt("tex0", 0);
            program->render(model);
        }
    };
}

#endif //MINECRAFT_CLIENT_BLOCK_RENDERER_H
