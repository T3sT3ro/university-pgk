//
// Created by tooster on 20.01.2020.
//

#include "GameController.hpp"
#include "Camera.hpp"

// TODO: squash into one update() method ???

Camera::Camera(vec3 position, vec3 forward, vec3 up)
        : position(position), forward(forward), up(up) {}

void Camera::use() { // FIXME: it's not optimal to do it every time. do a dirty flag or something
    mat4 vp[2] = {
            lookAt(position, position + forward, up),
            perspective(radians(fov), aspect, near, far)
    };

    glBindBuffer(GL_UNIFORM_BUFFER, GameController::matricesUBO);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(vp), vp);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
