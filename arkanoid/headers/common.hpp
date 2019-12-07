//
// Created by tooster on 05.11.2019.
//

#ifndef ARKANOID_COMMON_HPP
#define ARKANOID_COMMON_HPP

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#define NDEBUG
#ifdef NDEBUG
#define VERBOSE(x) do{x;} while(0)
#else
#define VERBOSE(x) {}
#endif

#define CDBG cerr << "[DEBUG]: "
#define CRITICAL(x) do{std::cerr<<(x)<<std::endl; exit(-1);}while(0)
#define BYTEPTR(ptr) ((char*)ptr)

#endif //ARKANOID_COMMON_HPP
