//
// Created by tooster on 20.01.2020.
//

#ifndef PGK_CAMERA_HPP
#define PGK_CAMERA_HPP

#include "common.hpp"
#include <vector>


using namespace glm;

/// every modification to
class Camera {

public:
    vec3 position,
         forward,
         up;
    float fov    = 75.0f,
          aspect = 4.0f / 3.0f,
          near   = 0.1f,
          far    = 100.0f;

    explicit Camera(vec3 position, vec3 forward = {0,0,1}, vec3 up = {0,1,0});


    void use();
};

#endif //PGK_CAMERA_HPP
