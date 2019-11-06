//
// Created by tooster on 05.11.2019.
//

#ifndef SHADER
#define SHADER

#include "common.hpp"

#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

class Shader {
    string vertPath, fragPath;
    string vertCode, fragCode;
    GLuint vertID = 0, fragID = 0;
    GLuint progID = 0;

public:
    Shader(const string &vertPath, const string &fragPath) : vertPath(vertPath), fragPath(fragPath) {}


    /// loads and compiles
    void reload() {
        clear();
        // load from files
        ifstream vFile, fFile;
        stringstream vStream, fStream;
        vFile.exceptions(ifstream::failbit | ifstream::badbit); fFile.exceptions(ifstream::failbit | ifstream::badbit);
        try {
            vFile.open(vertPath); vStream << vStream.rdbuf(); vertCode = vStream.str(); vFile.close();
            fFile.open(fragPath); fStream << fStream.rdbuf(); fragCode = fStream.str(); fFile.close();
        } catch (ifstream::failure &e) {
            cerr << "error while loading vertex or fragment shader file:\n" << e.what() << endl;
        }

        // compile
        vertID = glCreateShader(GL_VERTEX_SHADER); fragID = glCreateShader(GL_FRAGMENT_SHADER);

        GLint success = 0; GLchar infolog[GL_INFO_LOG_LENGTH];

        auto compileSingleShader = [&success, &infolog](GLint shaderID, const GLchar* codePtr) {
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

        progID = glCreateProgram();
        glAttachShader(progID, vertID);
        glAttachShader(progID, fragID);
        glLinkProgram(progID);

        glGetProgramiv(progID, GL_LINK_STATUS, &success);
        if(!success){
            glGetShaderInfoLog(progID, GL_INFO_LOG_LENGTH, nullptr, infolog);
            cerr << "shader compile error: " << infolog << endl;
        }
    }

    void use() {
        glUseProgram(progID);
    }

    virtual ~Shader() { clear(); }
private:


    void clear() {
        glDeleteShader(vertID);
        glDeleteShader(fragID);
        glDeleteProgram(progID);
    }
};

#endif