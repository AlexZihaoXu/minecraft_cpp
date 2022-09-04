//
// Created by Alex on 9/3/2022.
//

#ifndef MINECRAFT_WINDOW_H
#define MINECRAFT_WINDOW_H


#include "logging.h"

namespace engine {

    using namespace std::string_literals;

    class Window {
    private:
        int win_width, win_height;
        inline static bool created = false;
        std::string title, _display_title;
        GLFWwindow *windowHandle;
        bool vsync = false;
        int countedFPS = 0;
        double _settitle_timer = glfwGetTime();
        inline static Window *instance;

    public:


        Window(const std::string &title, int width, int height) {
            if (created) {
                throw std::runtime_error("Another window instance was created already!");
            }

            this->win_width = width;
            this->win_height = height;
            this->title = title;
            instance = this;

            created = true;

            if (!glfwInit()) {
                throw std::runtime_error("Unable to initialize GLFW system!");
            }
            console.info("GLFW System Initialized.");

            glfwDefaultWindowHints();
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
            windowHandle = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
            if (!windowHandle) {
                glfwTerminate();
                throw std::runtime_error("Unable to create GLFW window!");
            }

            glfwSetWindowSizeCallback(windowHandle, [](GLFWwindow *, int w, int h) {
                GLCall(glViewport(0, 0, w, h));
                instance->win_width = w;
                instance->win_height = h;
                instance->onResize(w, h);
            });
            glfwSetCursorPosCallback(windowHandle, [](GLFWwindow *, double x, double y) {
                instance->onMouseMotion(x, y);
            });
            glfwSetMouseButtonCallback(windowHandle, [](GLFWwindow *, int button, int action, int mods) {
                if (action == GLFW_PRESS) {
                    instance->onMouseButton(button, true);
                }
                if (action == GLFW_RELEASE) {
                    instance->onMouseButton(button, false);
                }
            });
            glfwSetKeyCallback(windowHandle, [](GLFWwindow *, int key, int scancode, int action, int mods) {
                if (action == GLFW_PRESS) {
                    instance->onKeyUpdate(key, true);
                }
                if (action == GLFW_RELEASE) {
                    instance->onKeyUpdate(key, false);
                }
            });
            glfwSetScrollCallback(windowHandle, [](GLFWwindow *, double x, double y) {
                instance->onScroll(x, y);
            });

            glfwMakeContextCurrent(windowHandle);
            gladLoadGL(glfwGetProcAddress);

            {
                console.info(std::string("GLFW Version: ") + glfwGetVersionString());
                int major;
                int minor;
                glfwGetVersion(&major, &minor, nullptr);
                console.info("OpenGL Version: "s + std::to_string(major) + "." + std::to_string(minor));
            }
            console.info("Window opened.");
        }

        ~Window() {
            glfwDestroyWindow(windowHandle);
            glfwTerminate();
            console.info("Window closed.");
        }

        void run() {
            glfwMakeContextCurrent(windowHandle);
            glfwSwapInterval(vsync ? 1 : 0);

            var lastRecord = glfwGetTime();
            GLCall(glViewport(0, 0, win_width, win_height));
            onSetup();

            std::deque<double> fpsCounter;

            while (!glfwWindowShouldClose(windowHandle)) {

                var now = glfwGetTime();
                var dt = now - lastRecord;
                lastRecord = now;
                fpsCounter.push_back(now);

                while (now - fpsCounter[0] > 1.0) {
                    fpsCounter.pop_front();
                }
                countedFPS = fpsCounter.size();

                if (now - _settitle_timer > 0.016) {
                    _settitle_timer = now;
                    if (_display_title != title) {
                        _display_title = title;
                        glfwSetWindowTitle(windowHandle, title.c_str());
                    }
                }

                this->onRender(dt);
                glfwSwapBuffers(windowHandle);
                glfwPollEvents();

                var error = glGetError();
                if (error != GL_NO_ERROR) {
                    console.err("[GL]: " + std::to_string(error) + " (uncaught)");
                }

            }
            onDestroy();

        }

        virtual void onResize(int w, int h) {

        }

        virtual void onMouseButton(int button, bool down) {

        }

        virtual void onMouseMotion(double x, double y) {

        }

        virtual void onKeyUpdate(int key, bool down) {

        }

        virtual void onScroll(double x, double y) {

        }

        virtual void onSetup() {

        }

        virtual void onRender(double dt) {

        }

        virtual void onDestroy() {

        }

        // Getters

        GLFWwindow *getHandle() {
            return windowHandle;
        }

        std::string getTitle() {
            return title;
        }

        int getFPS() const {
            return countedFPS;
        }

        bool getVsync() const {
            return vsync;
        }

        int getWidth() const {
            return win_width;
        }

        int getHeight() const {
            return win_height;
        }

        // Setters

        void setTitle(const std::string &new_title) {
            this->title = new_title;
        }

        void setVsync(bool enabled) {
            vsync = enabled;
            glfwSwapInterval(enabled ? 1 : 0);
        }
    };
}

#endif //MINECRAFT_WINDOW_H
