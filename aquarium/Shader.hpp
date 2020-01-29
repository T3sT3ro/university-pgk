//
// Created by tooster on 11.12.2019.
//

#ifndef SHADER_HPP
#define SHADER_HPP

#include "common.hpp"

using namespace std;

class Shader {
    static Shader *currentShader;

    GLuint progID = 0;
    string vertPath, fragPath;

    GLint fetchUniform(const char* name);
    void  saveBeforeReload();
    void  restoreAfterReload();
protected:
    Shader(const char *vertPath, const char *fragPath);

    virtual ~Shader();

public:

    // factory function
    static Shader *create(const char *vertPath, const char *fragPath);

    static Shader *getCurrentShader() {return currentShader;}

    GLuint getProgID() const { return progID; }
    string getName() const { return vertPath.substr(0, vertPath.find_last_of('.'));}

    void setUniformMat4(const char *name, mat4 &mat);
    void setUniformVec3(const char *name, vec3 &vec);
    void setUniformFloat(const char *name, float &value);

    /// loads and compiles/links the files
    void reload();

    void use();
};
#endif //SHADER_HPP
