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


#define VERBOSE(x) {}
#define VERBOSE_DEBUG(x) {}
#define VERBOSE_HIGH(x) {}
#define VERBOSE_LOW(x) VERBOSE(x)

#ifdef NDEBUG
    #undef VERBOSE
    #define VERBOSE(x) do{x;} while(0)
    #if defined(VERBOSITY_LOW)
        #undef VERBOSE_LOW
        #define VERBOSE_LOW(x) VERBOSE(x)
    #endif
    #if defined(VERBOSITY_HIGH)
        #undef VERBOSE_HIGH
        #define VERBOSE_HIGH(x) VERBOSE(x)
    #endif
    #if defined(VERBOSITY_DEBUG)
        #undef VERBOSE_DEBUG
        #define VERBOSE_DEBUG(x) VERBOSE(x)
    #endif
#endif

#define CDBG cerr << "[DEBUG]: "
#define CRITICAL(x) do{std::cerr<<(x)<<std::endl; exit(-1);}while(0)

using namespace std;
using namespace glm;

constexpr const float EPSILON = 10e-7;
constexpr const GLuint GLOB_MATRICES_BINDPOINT = 0;
constexpr const GLuint GLOB_LIGHTS_BINDPOINT = 1;

#define V_ATTRIB_NAME "position"
#define VN_ATTRIB_NAME "normal"
#define VT_ATTRIB_NAME "uv"

struct VPMatrices{
    mat4 viewMatrix;
    mat4 projectionMatrix;
};

#endif //COMMON_HPP
