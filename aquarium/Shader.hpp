//
// Created by tooster on 11.12.2019.
//

#ifndef SHADER_HPP
#define SHADER_HPP

#include "common.hpp"

using namespace std;

class Shader : public Usable {
    static Shader *currentShader;

    GLuint progID = 0;
    string vertPath, fragPath;

protected:
    Shader(const char *vertPath, const char *fragPath);

    virtual ~Shader();

public:

    // factory function
    static Shader *create(const char *vertPath, const char *fragPath);

    static Shader *getCurrentShader() {return currentShader;}

    GLuint getProgID() const { return progID; }
    string getName() const { return vertPath.substr(0, vertPath.find_last_of("."));}

    void setUniformMat4(const char* name, mat4 &mat);
    void setUniformVec3(const char *name, vec3 &vec);

    /// loads and compiles/links the files
    void reload();

    void use() override;
};

#include "Mesh.hpp"

/// Specifies the attributes that will be passed to current shader program;
/// shader must have attributes 'position' and 'mvp'
class Renderer {
    GLuint VAO;
    const GLuint instances;
    GLuint mvpVBO;
protected:
    Renderer(Shader *shader, Mesh *mesh, GLuint instances);

public:
    Shader *shader;
    Mesh   *mesh;

    static Renderer *create(Shader *shader, Mesh *mesh, GLuint instances = 1);

    void setModelMatrix(mat4 &modelMatrix, GLuint instance = 0); // instance works with instanced rendering

    void render();

    virtual ~Renderer();
};

#endif //SHADER_HPP
