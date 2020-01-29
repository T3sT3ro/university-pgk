//
// Created by tooster on 20.01.2020.
//

#include "Renderer.hpp"


Renderer::Renderer(Shader *shader, Mesh *mesh, GLuint instances)
        : instances(instances), shader(shader), mesh(mesh) {

    shader->use();

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    mesh->use(shader);

    GLuint mvpAttrib = glGetAttribLocation(shader->getProgID(), "modelMatrix");
    if (mvpAttrib == (GLuint) -1)
        cout << "[Renderer] " << shader->getName() << " `modelMatrix` attribute missing (maybe optimized-out)\n";
    else {
        vector<mat4> mats(instances);
        for(auto & mat : mats) mat = mat4(1.0);

        glGenBuffers(1, &modelMatrixVBO);
        glBindBuffer(GL_ARRAY_BUFFER, modelMatrixVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(mat4) * instances, mats.data(), GL_DYNAMIC_DRAW);

        for (size_t i = 0; i < 4; ++i) {
            glEnableVertexAttribArray(mvpAttrib + i);
            glVertexAttribPointer(mvpAttrib + i, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (const GLvoid *) (sizeof(vec4) * i));
            glVertexAttribDivisor(mvpAttrib + i, 1);
        }

    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

Renderer *Renderer::create(Shader *shader, Mesh *mesh, GLuint instances) {
    return new Renderer(shader, mesh, instances);
}

// sets the MVP, and if instance is specified it is done for that instance
void Renderer::setModelMatrix(mat4 &&modelMatrix, GLuint instance) {
    glBindBuffer(GL_ARRAY_BUFFER, modelMatrixVBO);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(mat4) * instance, sizeof(mat4), value_ptr(modelMatrix));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Renderer::render(Camera* camera) {
    shader->use();
    camera->use();
    if(texture) texture->use(); // use default texture instead (0) ???

    glBindVertexArray(VAO);

    if (backfaceCulling) {
        glEnable(GL_CULL_FACE);
        glCullFace(cullMode);
    } else
        glDisable(GL_CULL_FACE);

    if (wireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDrawElementsInstanced(mesh->mode, mesh->verticesCount, GL_UNSIGNED_SHORT, nullptr, instances);

    glBindVertexArray(0);
}

Renderer::~Renderer() {
    glDeleteBuffers(1, &modelMatrixVBO);
    glDeleteVertexArrays(1, &VAO);
}