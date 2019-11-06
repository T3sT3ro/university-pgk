//
// Created by tooster on 05.11.2019.
//

#ifndef ARKANOID_COMMON_HPP
#define ARKANOID_COMMON_HPP

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#ifdef NDEBUG
#define VERBOSE(x) do{x;} while(0)
#else
#define VERBOSE(x) {}
#endif

#define CRITICAL(x) do{std::cerr<<(x)<<std::endl; exit(-1);}while(0)

#endif //ARKANOID_COMMON_HPP
