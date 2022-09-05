//
// Created by Alex on 9/3/2022.
//

#ifndef MINECRAFT_ENGINE_H
#define MINECRAFT_ENGINE_H

#define var auto

#include <vector>
#include <deque>
#include <map>

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <string>
#include <stdexcept>
#include "logging.h"

void _glErrorCallback(const char *file, int line) {
    var errorCode = glGetError();
    if (errorCode != GL_NO_ERROR) {
        std::string error = "UNKNOWN";
        switch (errorCode) {
            case GL_INVALID_ENUM:
                error = "INVALID_ENUM";
                break;
            case GL_INVALID_VALUE:
                error = "INVALID_VALUE";
                break;
            case GL_INVALID_OPERATION:
                error = "INVALID_OPERATION";
                break;
            case GL_OUT_OF_MEMORY:
                error = "OUT_OF_MEMORY";
                break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                error = "INVALID_FRAMEBUFFER_OPERATION";
                break;
        }
        console.err("[GL]: error occurred: " + error + "(" + std::to_string(errorCode) + " in " + file + " at line " +
                    std::to_string(line));
    }
}

#define GLCall(statement) statement; _glErrorCallback(__FILE__, __LINE__)

#include "Window.h"
#include "buffers.h"
#include "shaders.h"
#include "textures.h"
#include "text-rendering.h"

#endif //MINECRAFT_ENGINE_H
