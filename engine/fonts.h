//
// Created by Alex on 9/6/2022.
//

#ifndef MINECRAFT_FONTS_H
#define MINECRAFT_FONTS_H

namespace engine {
    struct Character {
        GLuint texture;
        glm::ivec2 size;
        glm::ivec2 bearing;
        GLint advance;
    };

    class Font {
    private:
        inline static FT_Library ft;
        inline static bool initialized = false;
        inline static bool cleaned = false;
        inline static std::map<std::string, Font *> fontMap;
        inline static VertexArrayObject *renderVAO;
        inline static VertexBufferObject *renderVBO;
        inline static ShaderProgram *renderShader;
        int currentSize = 16;
        std::map<int, std::map<FT_ULong, Character>> characters;

        Font(const std::string &path) {
            init();
            if (FT_New_Face(ft, path.c_str(), 0, &face)) {
                throw std::runtime_error("Unable to load font from: " + path);
            }
            FT_Set_Pixel_Sizes(face, 0, currentSize);
        }

        ~Font() {
            FT_Done_Face(face);

        }

        void setFontSize(int size) {
            if (size != currentSize) {
                currentSize = size;
                FT_Set_Pixel_Sizes(face, 0, currentSize);
            }
        }

    public:

        static void init() {
            if (initialized)
                return;
            if (FT_Init_FreeType(&ft)) {
                throw std::runtime_error("Unable to initialize FreeType library!");
            }
            renderVBO = new VertexBufferObject({0, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 0});
            renderVAO = new VertexArrayObject(renderVBO, {2});
            var vertShader = new Shader(GL_VERTEX_SHADER, R"(
#version 330 core

layout (location=0) in vec2 aPos;
out vec2 texCoords;

uniform mat4 trans;

void main() {
    gl_Position = trans * vec4(aPos, 0.0, 1.0);
    texCoords = aPos;
}
)");
            var fragShader = new Shader(GL_FRAGMENT_SHADER, R"(
#version 330 core

in vec2 texCoords;
out vec4 FragColor;
uniform sampler2D tex;
uniform vec4 color;

void main() {
    FragColor = texture(tex, vec2(texCoords.x, 1 - texCoords.y)).r * color;
    if (FragColor.a < 0.01) {
        discard;
    }
}
)");

            renderShader = (new ShaderProgram())->attachShader(vertShader).attachShader(fragShader).link();

            delete vertShader;
            delete fragShader;
            initialized = true;

        }

        static void cleanup() {
            if (cleaned)
                return;

            delete renderVAO;
            delete renderVBO;
            delete renderShader;

            FT_Done_FreeType(ft);
            cleaned = true;
        }

        static Font *get(const std::string &path) {
            if (!fontMap.contains(path)) {
                try {
                    var font = new Font(path);
                    fontMap[path] = font;
                } catch (std::runtime_error& err) {
                    fontMap[path] = nullptr;
                    return nullptr;
                }
            }
            return fontMap[path];
        }

        FT_Face face;

        int getHeight(int font_size) {
            setFontSize(font_size);
            int bbox_ymax = FT_MulFix(face->bbox.yMax, face->size->metrics.y_scale) >> 6;
            int bbox_ymin = FT_MulFix(face->bbox.yMin, face->size->metrics.y_scale) >> 6;
            int height = bbox_ymax - bbox_ymin;
            return height + 2;
        }

        bool loadGlyph(FT_ULong c, int size) {
            if (characters.contains(size) && characters[size].contains(c)) {
                return true;
            }
            setFontSize(size);
            if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
                return false;
            }

            GLuint texture;
            GLCall(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
            GLCall(glGenTextures(1, &texture));
            GLCall(glBindTexture(GL_TEXTURE_2D, texture));
            GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0,
                                GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer));
            GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
            GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
            GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
            GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
            GLCall(glPixelStorei(GL_UNPACK_ALIGNMENT, 4));

            characters[size][c] = {
                    texture,
                    glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                    glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                    (GLint) face->glyph->advance.x
            };
            return true;
        }

        void render(const std::string &text, const glm::vec4 &color, int size, const glm::mat4 &trans = glm::mat4(1)) {

            float x = 0;
            float y = 0;

            init();
            renderShader->use();
            renderShader->setVec4("color", color);
            renderShader->setInt("tex", 0);
            renderVAO->bind();
            std::string::const_iterator c;
            GLCall(glActiveTexture(GL_TEXTURE0));
            for (c = text.begin(); c != text.end(); c++) {
                if (!loadGlyph(*c, size)) {
                    continue;
                }
                Character &ch = characters[size][*c];

                float xPos = x + ch.bearing.x;
                float yPos = y - (ch.size.y - ch.bearing.y);

                float w = ch.size.x;
                float h = ch.size.y;

                var mat = trans * glm::scale(
                        glm::translate(
                                glm::mat4(1.0f), {xPos, yPos, 0.0f}
                        ), {(float) w, (float) h, 1.0f});

                GLCall(glBindTexture(GL_TEXTURE_2D, ch.texture));
                renderShader->setMat4("trans", mat);
                GLCall(glDrawArrays(GL_TRIANGLES, 0, 6));
                x += (ch.advance >> 6);
            }
        }

        float getWidth(const std::string &text, int size) {
            std::string::const_iterator c;
            float x = 0;
            float w;
            for (c = text.begin(); c != text.end(); c++) {
                if (!loadGlyph(*c, size)) {
                    continue;
                }
                Character &ch = characters[size][*c];

                w = ch.bearing.x;
                x += (ch.advance >> 6);
            }
            return x + w;
        }
    };
}
#endif //MINECRAFT_FONTS_H
