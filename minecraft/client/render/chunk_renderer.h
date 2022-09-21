//
// Created by Alex on 9/17/2022.
//

#ifndef MINECRAFT_CHUNK_RENDERER_H
#define MINECRAFT_CHUNK_RENDERER_H

namespace minecraft::client::render {
    class ChunkSectionRenderer {
    private:
        blocks::ChunkSection *section;
        long changeID;
        engine::VertexArrayObject *vao = nullptr;
        engine::VertexBufferObject *vbo = nullptr;
        engine::ElementBufferObject *ebo = nullptr;
        inline static engine::ShaderProgram *shader = nullptr;

        static float map_value(float x, float begin, float end) {
            return begin + x * (end - begin);
        }

        inline static std::hash<glm::vec3> hasherv3f;

    public:
        ChunkSectionRenderer(blocks::ChunkSection *section) {
            this->section = section;
            changeID = -1;

            if (shader == nullptr) {
                var vertShader = new engine::Shader(GL_VERTEX_SHADER, R"(
#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in vec4 aColor;

out vec2 texCoords;
out vec4 color;

uniform mat4 trans;

void main() {
    texCoords = aTexCoords;
    color = aColor;
    gl_Position = trans *  vec4(aPos, 1.0);
}
)");
                var fragShader = new engine::Shader(GL_FRAGMENT_SHADER, R"(
#version 330 core

uniform sampler2D tex0;

in vec2 texCoords;
in vec4 color;
out vec4 FragColor;

void main() {
    FragColor = texture(tex0, texCoords) * color;
    if (FragColor.a < 0.1) {
        discard;
    }
}
)");
                shader = (new engine::ShaderProgram())->attachShader(vertShader).attachShader(fragShader).link();
                delete vertShader;
                delete fragShader;
            }
        }

        ~ChunkSectionRenderer() {
            if (vao != nullptr) {
                delete ebo;
                delete vbo;
                delete vao;
            }
        }

        blocks::BlockState *getState(int x, int y, int z) {
            if (0 <= x && x < 16 && 0 <= y && y < 16 && 0 <= z && z < 16) {
                return &section->getState(x, y, z);
            }
            return nullptr;
        }

        void addPoint(std::vector<GLfloat> &vertices, std::vector<GLuint> &indices,
                      std::map<unsigned long long, GLuint> &indicesMap,
                      int *indicesCount,
                      glm::vec3 pos, glm::vec2 texCoords, glm::vec4 color) {

            unsigned long long key1 = hasherv3f(pos);
            unsigned long long key2 = hasherv3f({texCoords.x, texCoords.y, color.x});
            unsigned long long key3 = hasherv3f({color.y, color.z, color.w});
            unsigned long long key = ((key1 * 23 + key2) * 17) + key3;


            if (!indicesMap.contains(key)) {
                vertices.push_back(pos.x);
                vertices.push_back(pos.y);
                vertices.push_back(pos.z);

                vertices.push_back(texCoords.x);
                vertices.push_back(texCoords.y);

                vertices.push_back(color.x);
                vertices.push_back(color.y);
                vertices.push_back(color.z);
                vertices.push_back(color.w);

                indicesMap[key] = *indicesCount;
                (*indicesCount)++;
            }
            indices.push_back(indicesMap[key]);
        }

        void update() {
            if (section->getChangeID() == changeID) return;
            double time = glfwGetTime();
            changeID = section->getChangeID();
            // 3 * 3D_Pos  2 * texCoords  4 * vertColor
            std::vector<GLfloat> vertices;
            std::vector<GLuint> indices;
            std::map<unsigned long long, GLuint> indicesMap;
            int indicesCount = 0;

            std::vector<int> directions = {
                    NORTH, SOUTH, EAST, WEST, UP, DOWN
            };

            std::vector<glm::vec4> brightnesses = {
                    glm::vec4{0.8, 0.8, 0.8, 1.0},
                    glm::vec4{0.6, 0.6, 0.6, 1.0},
                    glm::vec4{0.8, 0.8, 0.8, 1.0},
                    glm::vec4{0.6, 0.6, 0.6, 1.0},
                    glm::vec4{1.0, 1.0, 1.0, 1.0},
                    glm::vec4{0.5, 0.5, 0.5, 1.0},
            };

            for (int x = 0; x < 16; ++x) {
                for (int y = 0; y < 16; ++y) {
                    for (int z = 0; z < 16; ++z) {
                        var state = getState(x, y, z);
                        if (state == nullptr)
                            continue;
                        if (state->getBlock() == nullptr)
                            continue;
                        var offset = glm::vec3(x, y, z);
                        for (var def: state->getBlock()->getModelFaceRenderDefinition()) {
                            bool skip = false;
                            if (def.direction != DONT_CARE) {
                                for (var direction: directions) {
                                    if (direction == def.direction) {
                                        var next = direction2Vec[direction] + offset;
                                        var nextState = getState(next.x, next.y, next.z);
                                        if (nextState != nullptr) {
                                            if (nextState->getBlock() != nullptr &&
                                                nextState->getBlock()->getID() != 0) {
                                                skip = true;
                                            }
                                        }
                                        break;
                                    }
                                }
                            }
                            if (skip) continue;
                            var brightness = def.direction == DONT_CARE ? glm::vec4{1.0, 1.0, 1.0, 1.0}
                                                                        : brightnesses[def.direction];
                            var texRange = BlockRenderer::getTextureAtlasRangeTable()[def.texturePath];
                            addPoint(
                                    vertices, indices, indicesMap, &indicesCount,
                                    def.point1.modelCoords + offset, {
                                            map_value(def.point1.texCoords.x, texRange[0], texRange[2]),
                                            map_value(def.point1.texCoords.y, texRange[1], texRange[3]),
                                    }, brightness);
                            addPoint(
                                    vertices, indices, indicesMap, &indicesCount,
                                    def.point2.modelCoords + offset, {
                                            map_value(def.point2.texCoords.x, texRange[0], texRange[2]),
                                            map_value(def.point2.texCoords.y, texRange[1], texRange[3]),
                                    }, brightness);
                            addPoint(
                                    vertices, indices, indicesMap, &indicesCount,
                                    def.point3.modelCoords + offset, {
                                            map_value(def.point3.texCoords.x, texRange[0], texRange[2]),
                                            map_value(def.point3.texCoords.y, texRange[1], texRange[3]),
                                    }, brightness);
                        }
                    }
                }
            }

            if (vao != nullptr) {
                delete ebo;
                delete vbo;
                delete vao;
                ebo = nullptr;
                vbo = nullptr;
                vao = nullptr;
            }

            vbo = new engine::VertexBufferObject(vertices);
            ebo = new engine::ElementBufferObject(indices);
            vao = new engine::VertexArrayObject(vbo, ebo, {3, 2, 4});

            console.info("DEBUG", "Section update cost: " + std::to_string(glfwGetTime() - time) + "ms");
        }

        void render(glm::mat4 trans) {
            if (!vao) return;
            GLCall(glEnable(GL_DEPTH_TEST));
            GLCall(glEnable(GL_CULL_FACE));
            GLCall(glFrontFace(GL_CW));

            shader->use();
            shader->setMat4("trans", trans);
            GLCall(glActiveTexture(GL_TEXTURE0));
            BlockRenderer::getTextureAtlasBlocks()->bind();
            shader->setInt("tex0", 0);
            shader->render(vao);
        }
    };

    class ChunkRenderer {
    private:
        blocks::Chunk *chunk;
        std::vector<ChunkSectionRenderer *> sectionRenderers;
    public:
        ChunkRenderer(blocks::Chunk *chunk) {
            this->chunk = chunk;
            for (int i = 0; i < 16; ++i) {
                sectionRenderers.push_back(nullptr);
            }
        }

        ~ChunkRenderer() {
            for (int i = 0; i < 16; ++i) {
                if (sectionRenderers[i] != nullptr)
                    delete sectionRenderers[i];
            }
        }

        void update() {

            for (int i = 0; i < 16; ++i) {
                if (chunk->hasSection(i) && sectionRenderers[i] == nullptr) {
                    sectionRenderers[i] = new ChunkSectionRenderer(chunk->getSection(i));
                }
                if (sectionRenderers[i] != nullptr) {
                    sectionRenderers[i]->update();
                }
            }

        }

        void render(glm::mat4 trans) {
            for (int i = 0; i < 16; ++i) {
                if (sectionRenderers[i] != nullptr) {
                    var mat = glm::translate(trans, {0, 16 * i, 0});
                    sectionRenderers[i]->render(mat);
                }
            }
        }


    };
}

#endif //MINECRAFT_CHUNK_RENDERER_H
