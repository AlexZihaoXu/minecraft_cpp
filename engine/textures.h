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

    class Framebuffer {
    private:
        GLuint framebuffer, colorBuffer, renderBuffer;
        int w, h;

    public:
        Framebuffer(int w, int h) {
            this->w = w;
            this->h = h;
            GLCall(glGenFramebuffers(1, &framebuffer));
            bindContext();
            GLCall(glGenTextures(1, &colorBuffer));
            bind();
            GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr));
            GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
            GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
            GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer, 0));

            GLCall(glGenRenderbuffers(1, &renderBuffer));
            GLCall(glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer));
            GLCall(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h));
            GLCall(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER,
                                             renderBuffer));

            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
                throw std::runtime_error("Unable to create framebuffer!");
            }

            GLCall(glClearColor(0, 0, 0, 0));
            GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

            unbindContext();
            unbind();
        }

        ~Framebuffer() {
            GLCall(glDeleteFramebuffers(1, &framebuffer));
            GLCall(glDeleteRenderbuffers(1, &renderBuffer));
            GLCall(glDeleteTextures(1, &colorBuffer));
        }

        int width() const {
            return w;
        }

        int height() const {
            return h;
        }

        void bind() const {
            GLCall(glBindTexture(GL_TEXTURE_2D, colorBuffer));
        }

        void unbind() const {
            GLCall(glBindTexture(GL_TEXTURE_2D, 0));
        }

        void bindContext() const {
            GLCall(glBindFramebuffer(GL_FRAMEBUFFER, framebuffer));
        }

        void unbindContext() const {
            GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
        }
    };
}

#endif //MINECRAFT_TEXTURES_H
