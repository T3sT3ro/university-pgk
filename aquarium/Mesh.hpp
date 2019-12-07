//
// Created by tooster on 17.11.2019.
//

#ifndef ARKANOID_MESH_HPP
#define ARKANOID_MESH_HPP

#include "common.hpp"
#include "Shader.cpp"
#include <utility>
#include <vector>
#include <map>
#include <algorithm>

using namespace glm;


class Mesh {
    GLenum                       mode;
    GLuint                       vbo = 0, idx = 0; // for vertices

    struct attribData {
        size_t attSize;
        GLuint bufferObject;
    };
    std::map<GLuint, attribData> vaaMap;

public:
    const std::vector<vec3>     points;
    const std::vector<GLushort> indices;

    Mesh(vector<vec3> points, vector<GLushort> indices, GLenum usage = GL_STATIC_DRAW, GLenum mode = GL_TRIANGLES)
            : points(std::move(points)), indices(std::move(indices)), mode(mode) {

        glCreateVertexArrays(1, &vao);
        glBindVertexArray(vao);

        glGenBuffers(1, &vbo); // vertices buffer
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * points.size(), points.data(), usage);

        glGenBuffers(1, &idx); // indices buffer
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idx);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLushort), indices.data(), usage);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

        glBindVertexArray(0);
    }

    /// implicitly takes number of instances as buffer size
    template<typename T>
    void initAttrib(GLuint attrib, size_t components = 4, GLuint instances = 1, T *data = nullptr) {
        glBindVertexArray(vao);
        constexpr size_t attribSize = std::max((sizeof(T) + sizeof(vec4) - 1) / sizeof(vec4), 1ul); // round to ceil
        vaaMap[attrib] = {attribSize, 0};
        glGenBuffers(1, &vaaMap[attrib].bufferObject); // generate vbo for attrib
        glBindBuffer(GL_ARRAY_BUFFER, vaaMap[attrib].bufferObject);
        glBufferData(GL_ARRAY_BUFFER, sizeof(T) * instances, data, usage);
        for (size_t i = 0; i < attribSize; ++i) {
            glEnableVertexAttribArray(attrib + i);
            glVertexAttribPointer(attrib + i, components, GL_FLOAT, GL_FALSE,
                                  instances > 1 ? sizeof(T) : 0,
                                  instances > 1 ? (const GLvoid *) (sizeof(GLfloat) * components * i) : nullptr);
            if (instances > 1) glVertexAttribDivisor(attrib + i, 1);
        }

        glBindVertexArray(0);
    }

    template<typename T>
    void updateAttrib(GLuint attrib, T *data, int instance = 0) {
        glBindVertexArray(vao);
        GLuint buf = vaaMap[attrib].bufferObject;
        glBindBuffer(GL_ARRAY_BUFFER, buf);
        glBufferSubData(buf, sizeof(T) * instance, sizeof(T), data);

        glBindVertexArray(0);
    }

    void render(Shader &shader, GLuint instances = 1) {
        shader.use();
        glBindVertexArray(vao);
        glDrawElementsInstanced(mode, indices.size(), GL_UNSIGNED_INT, indices.data(), instances);
        glBindVertexArray(0);
    }

    ~Mesh() {
        glBindVertexArray(vao);
        glDeleteBuffers(1, &vbo);
        for (auto &iter : vaaMap) {
            for (size_t i = 0; i < iter.second.attSize; ++i) // disable all attributes
                glDisableVertexAttribArray(iter.first + i);
            glDeleteBuffers(1, &iter.second.bufferObject);
        }
        glDeleteVertexArrays(1, &vao);

        glBindVertexArray(0);
    }
};

#endif //ARKANOID_MESH_HPP
