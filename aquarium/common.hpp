//
// Created by tooster on 05.11.2019.
//

#ifndef COMMON_HPP
#define COMMON_HPP

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include <algorithm>
#include <iostream>
#include <string>


#define NDEBUG
#ifdef NDEBUG
#define VERBOSE(x) do{x;} while(0)
#else
#define VERBOSE(x) {}
#endif

#define CDBG cerr << "[DEBUG]: "
#define CRITICAL(x) do{std::cerr<<(x)<<std::endl; exit(-1);}while(0)
#define BYTEPTR(ptr) ((char*)ptr)

using namespace std;
using namespace glm;

class Usable {
public:
    virtual void use() = 0;
};

constexpr const float EPSILON = 10e-7;
constexpr const GLuint GLOB_MATRICES_BINDPOINT = 0;
constexpr const GLuint GLOB_LIGHTS_BINDPOINT = 1;

struct VPMatrices{
    mat4 viewMatrix;
    mat4 projectionMatrix;
};

#endif //COMMON_HPP
