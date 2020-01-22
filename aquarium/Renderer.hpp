//
// Created by tooster on 20.01.2020.
//

#ifndef PGK_RENDERER_HPP
#define PGK_RENDERER_HPP

#include <functional>
#include "Shader.hpp"
#include "Mesh.hpp"
#include "Camera.hpp"

/// Specifies the attributes that will be passed to current shader program;
/// shader must have attributes 'position' and 'mvp'
class Renderer {
    GLuint       VAO;
    const GLuint instances;
    GLuint       modelMatrixVBO;
protected:
    Renderer(Shader *shader, Mesh *mesh, GLuint instances);

public:
    Shader *shader;
    Mesh   *mesh;
    bool   backfaceCulling = true;
    GLenum cullMode        = GL_BACK;
    bool   wireframe       = false;

    static Renderer *create(Shader *shader, Mesh *mesh, GLuint instances = 1);

    void setModelMatrix(mat4 &&modelMatrix, GLuint instance = 0); // instance works with instanced rendering

    void render(Camera *camera);

    virtual ~Renderer();
};

#include "Shader.hpp"

#endif //PGK_RENDERER_HPP
