//
// Created by tooster on 05.11.2019.
//

#ifndef GAMECONTROLLER
#define GAMECONTROLLER

#include "GameController.hpp"
#include "Icosphere.hpp"

using namespace std;
using namespace glm;

Bubble::Bubble(GLuint instanceno) : GameObject(&icosphere, instanceno) {
    transform.scale = vec3(1) * (3.0f + (rand() % 10) / 10.0f);
    velocity = vec3(0, (3.0f + (rand() % 10) / 10.0f), 0);
}

void Bubble::update(float dt) {
    transform.translation += velocity * speed * dt;
    if (transform.translation.z > 100) transform.translation.z = 0;
}

Player::Player() : GameObject(&icosphere) {camera.position = &transform.translation;}

void Player::update(float dt) {
    glfwPollEvents();
    velocity = vec3(0);
    if(glfwGetKey(GameController::instance->window, GLFW_KEY_W) == GLFW_PRESS) velocity += camera.forward;
    if(glfwGetKey(GameController::instance->window, GLFW_KEY_S) == GLFW_PRESS) velocity -= camera.forward;
    if(glfwGetKey(GameController::instance->window, GLFW_KEY_D) == GLFW_PRESS) velocity += cross(camera.forward, camera.up);
    if(glfwGetKey(GameController::instance->window, GLFW_KEY_A) == GLFW_PRESS) velocity -= cross(camera.forward, camera.up);
    if(glfwGetKey(GameController::instance->window, GLFW_KEY_SPACE) == GLFW_PRESS) velocity += camera.up;
    if(glfwGetKey(GameController::instance->window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) velocity -= camera.up;

    transform.translation += velocity * dt * speed; // * speed
    // todo arrowkeys for arcballing

}


GameController::GameController(GLFWwindow *window) : window(window) {
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);
}

void GameController::update(float dt) {
    glfwPollEvents();

    player.update(dt);
    for(Bubble& bubble : bubbles) {
        bubble.update(dt);
        if(player.collider*bubble.collider)
            cerr << "collide\n";
    }
}

void GameController::render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for(Bubble& bubble : bubbles) bubble.render();
    player.render();
}

#endif