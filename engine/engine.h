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

void _glErrorCallback(const char* file, int line) {
    var error = glGetError();
    if (error != GL_NO_ERROR) {
        console.err("[GL]: " + std::to_string(error) + " in " + file + " at line " + std::to_string(line));
    }
}

#define GLCall(statement) statement; _glErrorCallback(__FILE__, __LINE__)
#include "Window.h"
#include "buffers.h"
#include "shaders.h"

#endif //MINECRAFT_ENGINE_H
