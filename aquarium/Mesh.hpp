//
// Created by tooster on 17.11.2019.
//

#ifndef ARKANOID_MESH_HPP
#define ARKANOID_MESH_HPP

#include "common.hpp"
#include <utility>
#include <vector>
#include <map>
#include <algorithm>

using namespace glm;


class Mesh {

    GLuint vbo, idx; // for vertices
public:
    const GLenum                mode;
    const std::vector<vec3>     points;
    const std::vector<GLushort> indices;

    Mesh(std::vector<vec3> points, std::vector<GLushort> indices,
         GLenum usage = GL_STATIC_DRAW, GLenum mode = GL_TRIANGLES)
            : points(std::move(points)), indices(std::move(indices)), mode(mode) {

        glGenBuffers(1, &vbo); // vertices buffer
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * points.size(), points.data(), usage);

        glGenBuffers(1, &idx); // indices buffer
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idx);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLushort), indices.data(), usage);
    }

    void use() {
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idx);
    }

    virtual ~Mesh() {
        glDeleteBuffers(1, &vbo);
    }
}


};

#endif //ARKANOID_MESH_HPP
