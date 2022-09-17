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
        float nearClip = 0.1f;
        float farClip = 1000.0f;

        glm::mat4 projMat(float width, float height) const {
            return glm::translate(
                    glm::rotate(glm::rotate(
                            glm::perspective(glm::radians(fov), width / height, nearClip, farClip),
                            pitch, {-1, 0, 0}), yaw, {0, 1, 0}),
                    -position
            );
        }

        glm::mat4 projMat() const {
            return projMat((float) engine::Window::getInstance().getWidth(),
                           (float) engine::Window::getInstance().getHeight());
        }

        void goForwardFree(float distance) {
            goForward(glm::cos(pitch) * distance);
            goUp(glm::sin(pitch) * distance);
        }

        void goForward(float distance) {
            position.x -= glm::cos(yaw + glm::half_pi<float>()) * distance;
            position.z -= glm::sin(yaw + glm::half_pi<float>()) * distance;
        }

        void goRight(float distance) {
            position.x += glm::cos(yaw) * distance;
            position.z += glm::sin(yaw) * distance;
        }

        void goUp(float distance) {
            position.y += distance;
        }

        glm::vec3 frontXZ() {
            return {-glm::cos(yaw + glm::half_pi<float>()), 0, -glm::sin(yaw + glm::half_pi<float>())};
        }

        glm::vec3 rightXZ() {
            return {(float) glm::cos(yaw), 0.0f, (float) glm::sin(yaw)};
        }

    };
}

#include "client_2d_renderer.h"
#include "client_block_renderer.h"

#endif //MINECRAFT_CLIENT_RENDER_H
