//
// Created by tooster on 17.11.2019.
//

#ifndef ARKANOID_MESH_HPP
#define ARKANOID_MESH_HPP

#include "common.hpp"
#include <vector>


using namespace glm;
using namespace std;


class Mesh : public Usable {
    GLuint vbo, idx; // for vertices

protected:
    Mesh(std::vector<vec3> &points, std::vector<GLushort> &indices, GLenum usage, GLenum mode);

public:
    const GLenum                mode, usage;
    std::vector<vec3>     points;
    std::vector<GLushort> indices;

public:
    static Mesh *create(vector<vec3> &points, vector<GLushort> &indices,
            GLenum usage = GL_STATIC_DRAW, GLenum mode = GL_TRIANGLES);

    static Mesh *create(const char * objPath, GLenum usage = GL_STATIC_DRAW, GLenum mode = GL_TRIANGLES);

    void use() override;

public:
    virtual ~Mesh();
};


#endif //ARKANOID_MESH_HPP
