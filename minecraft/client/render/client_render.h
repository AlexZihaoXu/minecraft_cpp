//
// Created by Alex on 9/11/2022.
//

#ifndef MINECRAFT_CLIENT_RENDER_H
#define MINECRAFT_CLIENT_RENDER_H

#include <vector>
#include <stack>

namespace minecraft::client::render {
    class Camera {
    private:
    public:
        glm::vec3 position{0, 0, 0};
        float pitch = 0;
        float yaw = 0;
        float fov = 90;

        glm::mat4 projMat(float width, float height) const {
            return glm::perspective(glm::radians(fov), width / height, 0.01f, 1000.0f);
        }

        glm::mat4 projMat() const {
            return projMat((float) engine::Window::getInstance().getWidth(),
                           (float) engine::Window::getInstance().getHeight());
        }

    };
}

#include "client_2d_renderer.h"
#include "client_block_renderer.h"

#endif //MINECRAFT_CLIENT_RENDER_H
