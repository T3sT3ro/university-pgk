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

class Usable {
public:
    virtual void use() = 0;
};

constexpr const float EPSILON = 10e-7;

#endif //COMMON_HPP
