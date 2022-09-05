//
// Created by Alex on 9/3/2022.
//

#ifndef MINECRAFT_SHADERS_H
#define MINECRAFT_SHADERS_H

#include <glad/gl.h>
#include <gtc/type_ptr.hpp>

namespace engine {
    class Shader {
    private:
        GLuint shaderID;
    public:
        Shader(int shaderType, std::string source) {
            shaderID = glCreateShader(shaderType);
            if (shaderID == 0) {
                throw std::runtime_error("Unable to create shader!");
            }
            var code = source.c_str();
            GLCall(glShaderSource(shaderID, 1, &code, nullptr));
            GLCall(glCompileShader(shaderID));
            GLint compiled;
            GLCall(glGetShaderiv(shaderID, GL_COMPILE_STATUS, &compiled));
            if (compiled != GL_TRUE) {
                GLsizei log_length = 0;
                GLchar message[1024];
                GLCall(glGetShaderInfoLog(shaderID, 1024, &log_length, message));
                console.err("Unable to compile shader: \n"s + message);
                throw std::runtime_error("Unable to compile shader!");
            }
        }

        GLuint getID() const {
            return shaderID;
        }

        ~Shader() {
            GLCall(glDeleteShader(shaderID));
        }
    };

    class ShaderProgram {
    private:
        GLuint programID;
        std::map<std::string, GLuint> uniformLocationMap;
    public:
        ShaderProgram() {
            programID = GLCall(glCreateProgram());
            if (programID == 0) {
                throw std::runtime_error("Unable to create shader program!");
            }
        }

        ~ShaderProgram() {
            GLCall(glDeleteProgram(programID));
        }

        ShaderProgram &attachShader(const Shader &shader) {
            GLCall(glAttachShader(programID, shader.getID()));
            return *this;
        }

        ShaderProgram &attachShader(const Shader *shader) {
            GLCall(glAttachShader(programID, shader->getID()));
            return *this;
        }

        void link() {
            GLCall(glLinkProgram(programID));
            GLint isLinked = 0;
            GLCall(glGetProgramiv(programID, GL_LINK_STATUS, &isLinked));

            if (isLinked == GL_FALSE) {
                GLint maxLength = 0;
                GLCall(glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &maxLength));
                GLchar infoLog[maxLength];
                GLCall(glGetProgramInfoLog(programID, maxLength, &maxLength, infoLog));

                console.err("Unable to link shader program: \n"s + infoLog);
                throw std::runtime_error("Unable to link shader program!");
            }
        }

        void use() {
            GLCall(glUseProgram(programID));
        }

        GLuint getUniformLocation(const std::string &uniform) {
            if (!uniformLocationMap.contains(uniform)) {
                uniformLocationMap[uniform] = glGetUniformLocation(programID, uniform.c_str());
            }
            return uniformLocationMap[uniform];
        }

        ShaderProgram &setMat4(const std::string uniform, glm::mat4 &mat) {
            GLuint location = getUniformLocation(uniform);
            use();
            GLCall(glUniformMatrix4fv(location, 1, GL_FALSE,
                                      glm::value_ptr(mat)));
            return *this;
        }

        ShaderProgram &setVec4(const std::string &uniform, glm::vec4 &vec) {
            GLuint location = getUniformLocation(uniform);
            use();
            GLCall(glUniform4fv(location, 1, glm::value_ptr(vec)));
            return *this;
        }

        ShaderProgram &setVec3(const std::string &uniform, glm::vec3 &vec) {
            GLuint location = getUniformLocation(uniform);
            use();
            GLCall(glUniform3fv(location, 1, glm::value_ptr(vec)));
            return *this;
        }

        ShaderProgram &setVec2(const std::string &uniform, glm::vec2 &vec) {
            GLuint location = getUniformLocation(uniform);
            use();
            GLCall(glUniform2fv(location, 1, glm::value_ptr(vec)));
            return *this;
        }

        ShaderProgram &setFloat(const std::string &uniform, GLfloat v) {
            GLuint location = getUniformLocation(uniform);
            use();
            GLCall(glUniform1fv(location, 1, &v));
            return *this;
        }

        ShaderProgram &setInt(const std::string &uniform, GLint v) {
            GLuint location = getUniformLocation(uniform);
            use();
            GLCall(glUniform1iv(location, 1, &v));
            return *this;
        }

        void render(const engine::VertexArrayObject *vao) {
            use();
            vao->render();
        }

    };
}

#endif //MINECRAFT_SHADERS_H
