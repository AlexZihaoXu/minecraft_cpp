//
// Created by Alex on 9/3/2022.
//

#ifndef MINECRAFT_BUFFERS_H
#define MINECRAFT_BUFFERS_H

namespace engine {


    class VertexBufferObject {
    private:
        GLuint bufferID;
        int dataSize;

    public:
        VertexBufferObject(GLfloat vertices[], GLint length) {
            GLCall(glGenBuffers(1, &bufferID));
            if (bufferID == 0) {
                throw std::runtime_error("Unable to create VBO!");
            }
            bind();
            dataSize = length;
            GLCall(glBufferData(GL_ARRAY_BUFFER, length * sizeof(GLfloat), vertices, GL_STATIC_DRAW));
            unbind();
        }

        VertexBufferObject(const std::vector<GLfloat> &vertices) {
            GLCall(glGenBuffers(1, &bufferID));
            if (bufferID == 0) {
                throw std::runtime_error("Unable to create VBO!");
            }
            GLfloat vertices_array[vertices.size()];
            for (int i = 0; i < vertices.size(); ++i) {
                vertices_array[i] = vertices[i];
            }
            bind();
            dataSize = vertices.size();
            GLCall(glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices_array, GL_STATIC_DRAW));
            unbind();
        }

        int size() const {
            return dataSize;
        }

        ~VertexBufferObject() {
            GLCall(glDeleteBuffers(1, &bufferID));
        }

        void bind() const {
            GLCall(glBindBuffer(GL_ARRAY_BUFFER, bufferID));
        }

        void unbind() const {
            GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
        }

    };

    class ElementBufferObject {
    private:
        GLuint id;
        int dataSize;
    public:

        int size() const {
            return dataSize;
        }

        ElementBufferObject(GLuint indices[], int length) {
            GLCall(glGenBuffers(1, &id));
            if (id == 0) {
                throw std::runtime_error("Unable to create EBO!");
            }
            bind();
            dataSize = length;
            GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, length * sizeof(GLfloat), indices, GL_STATIC_DRAW));
            unbind();
        }

        ElementBufferObject(const std::vector<GLuint> &indices) {
            dataSize = indices.size();
            GLCall(glGenBuffers(1, &id));
            if (id == 0) {
                throw std::runtime_error("Unable to create EBO!");
            }
            GLuint indices_array[dataSize];
            for (int i = 0; i < dataSize; ++i) {
                indices_array[i] = indices[i];
            }
            bind();
            GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLfloat), indices_array, GL_STATIC_DRAW));
            unbind();
        }

        ~ElementBufferObject() {
            GLCall(glDeleteBuffers(1, &id));
        }

        void bind() const {
            GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id));
        }

        void unbind() const {
            GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
        }
    };

    class VertexArrayObject {
    private:
        GLuint id;
        bool hasEbo = false;
        int length;
        const ElementBufferObject* ebo;

        void applyLayout(const std::vector<int> &sizes) {
            int stride = 0;
            GLintptr offset = 0;

            for (int i = 0; i < sizes.size(); ++i) {
                stride += sizes[i] * sizeof(GLfloat);
            }
            for (int i = 0; i < sizes.size(); ++i) {
                GLCall(glEnableVertexAttribArray(i));
                GLCall(glVertexAttribPointer(i, sizes[i], GL_FLOAT, GL_FALSE, stride, (GLvoid *) offset));
                offset += sizes[i] * sizeof(GLfloat);
            }
        }

        void initialized(const VertexBufferObject *vbo, const ElementBufferObject *ebo, const std::vector<int> &sizes) {
            GLCall(glGenVertexArrays(1, &id));
            GLCall(glBindVertexArray(id));
            vbo->bind();
            if (ebo != nullptr)
                ebo->bind();
            applyLayout(sizes);
        }

    public:
        VertexArrayObject(const VertexBufferObject *vbo, const std::vector<int> &sizes) {
            initialized(vbo, nullptr, sizes);
            int groupSize = 0;
            for (int i = 0; i < sizes.size(); ++i) {
                groupSize += sizes[i];
            }
            length = vbo->size() / groupSize;
        }

        VertexArrayObject(const VertexBufferObject *vbo, const ElementBufferObject *ebo,
                          const std::vector<int> &sizes) {
            initialized(vbo, ebo, sizes);
            length = ebo->size();
            hasEbo = true;
            this->ebo = ebo;
        }

        ~VertexArrayObject() {
            GLCall(glDeleteVertexArrays(1, &id));
        }

        void bind() const {
            GLCall(glBindVertexArray(id));
        }

        void render() const {
            bind();
            if (hasEbo) {
                ebo->bind();
                GLCall(glDrawElements(GL_TRIANGLES, length, GL_UNSIGNED_INT, 0));
            } else {
                GLCall(glDrawArrays(GL_TRIANGLES, 0, length));
            }
        }
    };


}

#endif //MINECRAFT_BUFFERS_H
