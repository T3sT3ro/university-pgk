//
// Created by tooster on 05.11.2019.
//

#ifndef SHADER
#define SHADER

#include "common.hpp"

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
    GLuint progID = 0;

    GLuint vao; // one vao per shader should suffice for now.

public:
    Shader(string vertPath, string fragPath) : vertPath(std::move(vertPath)), fragPath(std::move(fragPath)) {}


    /// loads and compiles
    void reload() {
        clear();
        // load from files
        ifstream vFile, fFile;
        stringstream vStream, fStream;
        vFile.exceptions(ifstream::failbit | ifstream::badbit); fFile.exceptions(ifstream::failbit | ifstream::badbit);
        try {
            vFile.open(vertPath); vStream << vFile.rdbuf(); vertCode = vStream.str(); vFile.close();
            fFile.open(fragPath); fStream << fFile.rdbuf(); fragCode = fStream.str(); fFile.close();
        } catch (ifstream::failure &e) {
            cerr << "error while loading vertex or fragment shader file:\n" << e.what() << endl;
        }
        if(vertCode.empty() || fragCode.empty()){
            cerr << "warning: " << (vertCode.empty() ? "vertex shader " : "")
            << (fragCode.empty() ? "fragment shader " : "") << "is empty" << endl;
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

        VERBOSE(CDBG << "vert:\n" << vertCode << "frag:\n" << fragCode << endl);
        glGetProgramiv(progID, GL_LINK_STATUS, &success);
        if(!success){
            glGetShaderInfoLog(progID, GL_INFO_LOG_LENGTH, nullptr, infolog);
            cerr << "shader link error: " << infolog << endl;
        }
    }

    void use() {
        if(currentProgID != progID){
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

#endif