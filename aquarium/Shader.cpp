//
// Created by tooster on 05.11.2019.
//

#include "Shader.hpp"
#include <fstream>
#include <sstream>
#include <utility>

//  region Shader
Shader *Shader::currentShader = nullptr;

Shader::Shader(const char *vertPath, const char *fragPath) : vertPath(vertPath), fragPath(fragPath) {
    progID = glCreateProgram();
    reload();
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
        vFile.open(vertPath); vStream << vFile.rdbuf(); vertCode = vStream.str(); vFile.close();
        fFile.open(fragPath); fStream << fFile.rdbuf(); fragCode = fStream.str(); fFile.close();
    } catch (ifstream::failure &e) {
        cerr << "[Shader] " << getName() << " error while loading vertex or fragment shader file:\n" << e.what() << endl;
    }
    if (vertCode.empty() || fragCode.empty()) {
        cerr << "[Shader] " << getName() << " warning: " << (vertCode.empty() ? ".vert code " : fragCode.empty() ? "and " : "")
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

    glDeleteShader(vertID);glUseProgram(progID);
    glDeleteShader(fragID);
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

Shader *Shader::create(const char *vertPath, const char *fragPath) {
    return new Shader(vertPath, fragPath);
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
        cerr << "[Renderer] " << shader->getName() << " `position` attribute not found\n";
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

//    GLuint mvpAttrib = glGetAttribLocation(shader->getProgID(), "mvp");
//    if (mvpAttrib == (GLuint) -1)
//        cout << "[Renderer] " << shader->getName() << " `mvp` attribute not found\n";
//
//    glGenBuffers(1, &mvpVBO);
//    glBindBuffer(GL_ARRAY_BUFFER, mvpVBO);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(mat4) * instances, nullptr, GL_DYNAMIC_DRAW);
//    for (size_t i = 0; i < 4; ++i) {
//        glEnableVertexAttribArray(mvpAttrib + i);
//        glVertexAttribPointer(mvpAttrib + i, 4, GL_FLOAT, GL_FALSE,
//                              instances == 1 ? 0 : 4,
//                              instances == 1 ? nullptr : (const GLvoid *) (sizeof(GLfloat) * 4 * i));
//        if (instances > 1) glVertexAttribDivisor(mvpAttrib + i, 1);
//    }

    glBindVertexArray(0);
    cerr.flush();
}

Renderer * Renderer::create(Shader * shader, Mesh * mesh, GLuint instances) {
    return new Renderer(shader, mesh, instances);
}

// sets the MVP, and if instance is specified it is done for that instance
void Renderer::setMVP(mat4 &mvp, GLuint instance) {
    glBindBuffer(GL_ARRAY_BUFFER, mvpVBO);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(mat4) * instance, sizeof(mat4), &mvp);
}

void Renderer::render() {
    shader->use();
    glBindVertexArray(VAO);
//    CDBG << mesh->indices.size() <<" "<< instances << "\n";
    glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr, 2);
    glBindVertexArray(0);
}

Renderer::~Renderer() {
    glDeleteBuffers(1, &mvpVBO);
    glDeleteVertexArrays(1, &VAO);
}
//endregion Renderer
