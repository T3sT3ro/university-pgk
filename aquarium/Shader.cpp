//
// Created by tooster on 05.11.2019.
//

#include "Shader.hpp"
#include <fstream>
#include <sstream>

Shader *Shader::currentShader = nullptr;

Shader::Shader(const char *vertPath, const char *fragPath) : vertPath(vertPath), fragPath(fragPath) {
    progID = glCreateProgram();
    reload();
}

Shader *Shader::create(const char *vertPath, const char *fragPath) {
    return new Shader(vertPath, fragPath);
}

void Shader::setUniformMat4(const char *name, mat4 &mat) {
    this->use();
    glUniformMatrix4fv(glGetUniformLocation(progID, name), 1, GL_FALSE, &mat[0][0]);
}

void Shader::setUniformVec3(const char *name, vec3 &vec){
    this->use();
    glUniform3fv(glGetUniformLocation(progID, name), 1, &vec[0]);
}

/// loads and compiles and links
void Shader::reload() {

    // load from files
    ifstream     vFile, fFile;
    stringstream vStream, fStream;
    vFile.exceptions(ifstream::failbit | ifstream::badbit);
    fFile.exceptions(ifstream::failbit | ifstream::badbit);
    string vertCode, fragCode;
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
        cerr << "[Shader] " << getName() << " error while loading vertex or fragment shader file:\n" << e.what()
             << endl;
    }
    if (vertCode.empty() || fragCode.empty()) {
        cerr << "[Shader] " << getName() << " warning: "
             << (vertCode.empty() ? ".vert code " : fragCode.empty() ? "and " : "")
             << (fragCode.empty() ? ".frag code " : "") << "empty\n";
    }
    GLint  success = 0;
    GLchar infolog[GL_INFO_LOG_LENGTH];

    GLuint vertID = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragID = glCreateShader(GL_FRAGMENT_SHADER);

    auto compileSingleShader = [&success, &infolog, this](GLint shaderID, const GLchar *codePtr) {
        glShaderSource(shaderID, 1, &codePtr, nullptr);
        glCompileShader(shaderID);
        glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shaderID, GL_INFO_LOG_LENGTH, nullptr, infolog);
            cerr << "[Shader] " << getName() << " compile error: " << infolog << endl;
        }
    };
    compileSingleShader(vertID, vertCode.c_str());
    compileSingleShader(fragID, fragCode.c_str());
    assert(vertID != fragID);

    glAttachShader(progID, vertID);
    glAttachShader(progID, fragID);
    glLinkProgram(progID);

    //VERBOSE(CDBG << "# vert:\n" << vertCode << "\n# frag:\n" << fragCode << endl);
    glGetProgramiv(progID, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(progID, GL_INFO_LOG_LENGTH, nullptr, infolog);
        cerr << "[Shader] " << getName() << " link error: " << infolog << endl;
    }


    glDetachShader(progID, vertID);
    glDetachShader(progID, fragID);

    glDeleteShader(vertID);
    glDeleteShader(fragID);
    glUseProgram(progID);

    int VPMatricesUBOIdx = glGetUniformBlockIndex(progID, "VPMatrices");
    int LightsUBOIdx = glGetUniformBlockIndex(progID, "Lights");

    glUniformBlockBinding(progID, VPMatricesUBOIdx, 0);
    glUniformBlockBinding(progID, LightsUBOIdx, 1);

    cerr << "[Shader] " << getName() << " reloaded\n";
}

/// cleverly swaps in the shader
void Shader::use() {
    if (currentShader == nullptr || currentShader->progID != progID) {
        currentShader = this;
        glUseProgram(progID);
    }
}

Shader::~Shader() {
    glDeleteProgram(progID);
    if (currentShader == this) currentShader = nullptr;
}
