//
// Created by tooster on 05.11.2019.
//

#ifndef SHADER
#define SHADER

#include "common.hpp"
#include "Mesh.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <utility>


using namespace std;

class Shader {
    static GLuint currentProgID;

    string vertPath, fragPath;
    string vertCode, fragCode;
    GLuint vertID = 0, fragID = 0;

    GLuint vao; // one vao per shader should suffice for now.

public:
    const GLuint progID;

    Shader(string vertPath, string fragPath)
            : vertPath(std::move(vertPath)), fragPath(std::move(fragPath)), progID(glCreateProgram()) {}


    /// loads and compiles
    void reload() {
        clear();
        // load from files
        ifstream     vFile, fFile;
        stringstream vStream, fStream;
        vFile.exceptions(ifstream::failbit | ifstream::badbit);
        fFile.exceptions(ifstream::failbit | ifstream::badbit);
        try {
            vFile.open(vertPath);
            vStream << vFile.rdbuf();
            vertCode = vStream.str();
            vFile.close();
            fFile.open(fragPath);
            fStream << fFile.rdbuf();
            fragCode = fStream.str();
            fFile.close();
        } catch (ifstream::failure &e) {
            cerr << "error while loading vertex or fragment shader file:\n" << e.what() << endl;
        }
        if (vertCode.empty() || fragCode.empty()) {
            cerr << "warning: " << (vertCode.empty() ? "vertex shader " : "")
                 << (fragCode.empty() ? "fragment shader " : "") << "is empty" << endl;
        }

        // compile
        vertID = glCreateShader(GL_VERTEX_SHADER);
        fragID = glCreateShader(GL_FRAGMENT_SHADER);

        GLint  success = 0;
        GLchar infolog[GL_INFO_LOG_LENGTH];

        auto compileSingleShader = [&success, &infolog](GLint shaderID, const GLchar *codePtr) {
            glShaderSource(shaderID, 1, &codePtr, nullptr);
            glCompileShader(shaderID);
            glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
            if (!success) {
                glGetShaderInfoLog(shaderID, GL_INFO_LOG_LENGTH, nullptr, infolog);
                cerr << "shader compile error: " << infolog << endl;
            }
        };
        compileSingleShader(vertID, vertCode.c_str());
        compileSingleShader(fragID, fragCode.c_str());
        assert(vertID != fragID);

        glAttachShader(progID, vertID);
        glAttachShader(progID, fragID);
        glLinkProgram(progID);

        VERBOSE(CDBG << "vert:\n" << vertCode << "frag:\n" << fragCode << endl);
        glGetProgramiv(progID, GL_LINK_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(progID, GL_INFO_LOG_LENGTH, nullptr, infolog);
            cerr << "shader link error: " << infolog << endl;
        }
    }

    void use() {
        if (currentProgID != progID) {
            glUseProgram(progID);
            currentProgID = progID;
        }
    }

    virtual ~Shader() { clear(); }

private:


    void clear() {
        glDeleteShader(vertID);
        glDeleteShader(fragID);
        glDeleteProgram(progID);
    }
};

/// Specifies the attributes that will be passed to current shader program;
/// shader must have attributes 'position' and 'mvp'
class Renderer {
    GLuint VAO;
    GLuint instances;
    Mesh   *mesh;
    Shader *shader;

    GLuint mvpVBO;

public:
    Renderer(Shader *shader, Mesh *mesh, GLuint instances = 1)
            : shader(shader), mesh(mesh), instances(instances) {
        glCreateVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        GLuint posAttrib = glGetAttribLocation(shader->progID, "position");
        mesh->use();
        glEnableVertexAttribArray(posAttrib);
        glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

        GLuint mvpAttrib = glGetAttribLocation(shader->progID, "mvp");
        glGenBuffers(1, &mvpVBO);
        glBindBuffer(GL_ARRAY_BUFFER, mvpVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(mat4) * instances, nullptr, GL_DYNAMIC_DRAW);
        for (size_t i = 0; i < 4; ++i) {
            glEnableVertexAttribArray(mvpAttrib + i);
            glVertexAttribPointer(mvpAttrib + i, 4, GL_FLOAT, GL_FALSE,
                                  instances == 1 ? 0 : 4,
                                  instances == 1 ? nullptr : (const GLvoid *) (sizeof(GLfloat) * 4 * i));
            if (instances > 1) glVertexAttribDivisor(mvpAttrib + i, 1);
        }

        glBindVertexArray(0);
    }

    // sets the MVP, and if instance is specified it is done for that instance
    void setMVP(mat4 &mvp, GLuint instance = 0) {
        glBindBuffer(GL_ARRAY_BUFFER, mvpVBO);
        glBufferSubData(GL_ARRAY_BUFFER, sizeof(mat4) * instance, sizeof(mat4), &mvp);
    }

    void render() {
        shader->use();
        glBindVertexArray(VAO);
        glDrawElementsInstanced(mesh->mode, mesh->indices.size(), GL_UNSIGNED_INT, mesh->indices.data(), instances);
        glBindVertexArray(0);
    }

    virtual ~Renderer() {
        glDeleteBuffers(1, &mvpVBO);
        glDeleteVertexArrays(1, &VAO);
    }
};

#endif