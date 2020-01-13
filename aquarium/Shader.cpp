//
// Created by tooster on 05.11.2019.
//

#include "Shader.hpp"
#include <fstream>
#include <sstream>

//  region Shader
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
        glGetShaderInfoLog(progID, GL_INFO_LOG_LENGTH, nullptr, infolog);
        cerr << "[Shader] " << getName() << " link error: " << infolog << endl;
    }


    glDetachShader(progID, vertID);
    glDetachShader(progID, fragID);

    glDeleteShader(vertID);
    glUseProgram(progID);
    glDeleteShader(fragID);

    int VPMatricesUBOIdx = glGetUniformBlockIndex(progID, "VPMatrices");
    int LightsUBOIdx = glGetUniformBlockIndex(progID, "Lights");
    cerr << "DBG: VPM=" << VPMatricesUBOIdx << " L=" << LightsUBOIdx << endl;
    glUniformBlockBinding(progID, VPMatricesUBOIdx, GLOB_MATRICES_BINDPOINT);
    glUniformBlockBinding(progID, LightsUBOIdx, GLOB_LIGHTS_BINDPOINT);

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

//endregion Shader




//region Renderer
/// Specifies the attributes that will be passed to current shader program;
/// shader must have attributes 'position' and 'mvp'
Renderer::Renderer(Shader *shader, Mesh *mesh, GLuint instances)
        : shader(shader), mesh(mesh), instances(instances) {

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    mesh->use();

    GLuint posAttrib = glGetAttribLocation(shader->getProgID(), "position");
    if (posAttrib == (GLuint) -1)
        cerr << "[Renderer] " << shader->getName() << " `position` attribute missing (maybe optimized-out)\n";
    else {
        glEnableVertexAttribArray(posAttrib);
        glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        if (instances > 1) glVertexAttribDivisor(posAttrib, 1);
    }

    GLuint mvpAttrib = glGetAttribLocation(shader->getProgID(), "modelMatrix");
    if (mvpAttrib == (GLuint) -1)
        cout << "[Renderer] " << shader->getName() << " `modelMatrix` attribute missing (maybe optimized-out)\n";
    else {
        auto identity = mat4(1);
        glGenBuffers(1, &mvpVBO);
        glBindBuffer(GL_ARRAY_BUFFER, mvpVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(mat4) * instances, &identity[0][0], GL_DYNAMIC_DRAW);
        for (size_t i = 0; i < 4; ++i) {
            glEnableVertexAttribArray(mvpAttrib + i);
            glVertexAttribPointer(mvpAttrib + i, 4, GL_FLOAT, GL_FALSE,
                                  /*instances == 1 ? 0 : */4,
                                  /*instances == 1 ? nullptr : */(const GLvoid *) (sizeof(GLfloat) * 4 * i));
            if (instances > 1) glVertexAttribDivisor(mvpAttrib + i, 1);
        }
    }

    glBindVertexArray(0);
    cerr.flush();
}

Renderer *Renderer::create(Shader *shader, Mesh *mesh, GLuint instances) {
    return new Renderer(shader, mesh, instances);
}

// sets the MVP, and if instance is specified it is done for that instance
void Renderer::setModelMatrix(mat4 &modelMatrix, GLuint instance) {
    glBindBuffer(GL_ARRAY_BUFFER, mvpVBO);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(mat4) * instance, sizeof(mat4), &modelMatrix);
}

void Renderer::render() {
    shader->use();
    glBindVertexArray(VAO);
//    CDBG << mesh->indices.size() <<" "<< instances << "\n";
    glDrawElementsInstanced(mesh->mode, mesh->indices.size(), GL_UNSIGNED_SHORT, nullptr, instances);
    glBindVertexArray(0);
}

Renderer::~Renderer() {
    glDeleteBuffers(1, &mvpVBO);
    glDeleteVertexArrays(1, &VAO);
}
//endregion Renderer
