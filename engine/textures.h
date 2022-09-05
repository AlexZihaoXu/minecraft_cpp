//
// Created by Alex on 9/4/2022.
//

#ifndef MINECRAFT_TEXTURES_H
#define MINECRAFT_TEXTURES_H

#define STB_IMAGE_IMPLEMENTATION

#include <stb_image.h>

namespace engine {
    class Texture {
    private:

        inline static std::map<std::string, Texture *> textureMap;

        GLuint id;
        int w, h, channel;

        Texture(const std::string &path) {
            GLCall(glGenTextures(1, &id));
            bind();
            GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
            GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
            GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST));
            GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));

            GLubyte *data = stbi_load(path.c_str(), &w, &h, &channel, 0);
            if (data) {
                GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data));
                GLCall(glGenerateMipmap(GL_TEXTURE_2D));
            } else {
                throw std::runtime_error("Unable to load texture from: " + path);
            }
            stbi_image_free(data);
        }

        ~Texture() {
            GLCall(glDeleteTextures(1, &id));
        }

    public:
        static Texture *get(const std::string &path) {
            if (!textureMap.contains(path)) {
                textureMap[path] = new Texture(path);
            }
            return textureMap[path];
        }

        int width() const {
            return w;
        }

        int height() const {
            return h;
        }

        int channels() const {
            return channel;
        }

        void bind() const {
            GLCall(glActiveTexture(GL_TEXTURE0));
            GLCall(glBindTexture(GL_TEXTURE_2D, id));
        }

        void unbind() const {
            GLCall(glBindTexture(GL_TEXTURE_2D, 0));
        }

    };
}

#endif //MINECRAFT_TEXTURES_H
