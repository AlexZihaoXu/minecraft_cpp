#include "minecraft/client/client.h"

class ViewController : public minecraft::client::WindowEventHandler {
public:
    minecraft::client::render::Camera *cam;
    minecraft::client::MinecraftClientWindow *win;
    bool locked = false;
    glm::vec3 vel{0, 0, 0};

    ViewController() {
        cam = &minecraft::client::MinecraftClientWindow::getInstance()->cam;
        win = minecraft::client::MinecraftClientWindow::getInstance();
    }

    void onMouseDown(int button) override {
        if (button == 0) {
            if (!locked) {
                locked = true;
                glfwSetInputMode(win->getHandle(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                glfwSetCursorPos(win->getHandle(), 0, 0);
            }
        }
    }

    void onFocusChange(bool focused) override {
        if (!focused) {
            if (locked) {
                unlock();
            }
        }
    }

    void onMouseMove(double x, double y) override {
        if (locked) {
            double sensitivity = 0.0035;
            glfwSetCursorPos(win->getHandle(), 0, 0);
            cam->yaw += x * sensitivity;
            cam->pitch -= y * sensitivity;
            cam->yaw = std::fmod(cam->yaw, 2.0f * glm::pi<float>());
            cam->pitch = glm::min(glm::half_pi<float>(), glm::max(-glm::half_pi<float>(), cam->pitch));
        }
    }

    void unlock() {
        locked = false;
        glfwSetInputMode(win->getHandle(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        glfwSetCursorPos(win->getHandle(), win->getWidth() / 2.0f, win->getHeight() / 2.0f);
    }

    void onKeydown(int key) override {
        if (key == GLFW_KEY_ESCAPE) {
            if (locked) {
                unlock();
            }
        }
    }

    void onRender(double dt) override {
        double speed = 50;
        if (locked) {
            if (glfwGetKey(win->getHandle(), GLFW_KEY_W)) {
                vel += (float) (speed * dt) * cam->frontXZ();
            }
            if (glfwGetKey(win->getHandle(), GLFW_KEY_S)) {
                vel -= (float) (speed * dt) * cam->frontXZ();
            }
            if (glfwGetKey(win->getHandle(), GLFW_KEY_D)) {
                vel += (float) (speed * dt) * cam->rightXZ();
            }
            if (glfwGetKey(win->getHandle(), GLFW_KEY_A)) {
                vel -= (float) (speed * dt) * cam->rightXZ();
            }
            if (glfwGetKey(win->getHandle(), GLFW_KEY_SPACE)) {
                vel.y += speed * dt;
            }
            if (glfwGetKey(win->getHandle(), GLFW_KEY_LEFT_SHIFT)) {
                vel.y -= speed * dt;
            }

        }

        vel += (glm::vec3(0) - vel) * glm::min(1.0f, (float) dt * 3);
        vel.y -= vel.y * glm::min(1.0f, (float) dt * 3);
        cam->position += vel * (float) dt;
    }
};

int main() {

    minecraft::client::MinecraftClientWindow::getInstance()->addEventHandler(new ViewController());
    minecraft::client::MinecraftClientWindow::launch();
    minecraft::client::MinecraftClientWindow::cleanup();

    return 0;
}
