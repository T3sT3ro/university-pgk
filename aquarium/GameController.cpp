//
// Created by tooster on 05.11.2019.
//

#ifndef GAMECONTROLLER
#define GAMECONTROLLER

#include "GameController.hpp"
#include "Icosphere.hpp"
#include "Shader.hpp"
#include <iostream>
#include <fstream>
#include <sstream>


void Camera::updateView() {
    glBindBuffer(GL_UNIFORM_BUFFER, GameController::matricesUBO);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(mat4), value_ptr(
            glm::lookAt(position, forward, up)));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Camera::updateProjection() {
    glBindBuffer(GL_UNIFORM_BUFFER, GameController::matricesUBO);
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(mat4), sizeof(mat4), value_ptr(
            glm::perspective(glm::radians(fov), aspect, near, far)));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

Camera::Camera(vec3 position, vec3 forward, vec3 up)
: position(position), forward(forward), up(up){
    updateView();
    updateProjection();
}




Bubble::Bubble(GLuint instanceno) : GameObject(instanceno) {
    transform.scale = vec3(1) * (3.0f + (rand() % 10) / 10.0f);
    velocity = vec3(0, (3.0f + (rand() % 10) / 10.0f), 0);
}

void Bubble::update(float dt) {
    transform.translation += velocity * speed * dt;
    if (transform.translation.z > 100) transform.translation.z = 0;
//    cerr << " x:" << transform.translation.x << " y:" << transform.translation.y << " z:" << transform.translation.x << endl;
}




Player::Player() : GameObject() {
    camera = new Camera();
    camera->position = transform.translation;
    camera->updateView();
}

void Player::update(float dt) {
    velocity = vec3(0);
    GLFWwindow *window = GameController::instance->window;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) velocity += camera->forward;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) velocity -= camera->forward;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) velocity += cross(camera->forward, camera->up);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) velocity -= cross(camera->forward, camera->up);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) velocity += camera->up;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) velocity -= camera->up;

    cerr << glm::to_string(velocity) << " transform: ";
    transform.translation += velocity * dt * speed;
    cerr << glm::to_string(transform.translation) << endl;
    mat4 rot = glm::identity<mat4>();
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        rot = glm::rotate(rot, angularSpeed*dt, {0,1,0});
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        rot = glm::rotate(rot, -angularSpeed*dt, {0,1,0});
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        rot = glm::rotate(rot, angularSpeed*dt, glm::cross(camera->forward, camera->up));
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        rot = glm::rotate(rot, -angularSpeed*dt, glm::cross(camera->forward, camera->up));

    camera->position = transform.translation;
    camera->forward = rot * vec4(camera->forward, 0);
    camera->up = rot * vec4(camera->up, 0);
    camera->updateView();

}




Aquarium::Aquarium() {
    // collision update to check if player inside collider
}




GameController *GameController::instance;
GLuint GameController::matricesUBO;
GLuint GameController::lightsUBO;

GameController::GameController(GLFWwindow *window) : window(window) {
    instance = this;

    // initializing global matrices UBO
    glGenBuffers(1, &matricesUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, matricesUBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(mat4)*2, nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, matricesUBO);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(mat4), value_ptr(mat4(1)));

    // initializing global lights UBO
    glGenBuffers(1, &lightsUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, lightsUBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::vec4), nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, lightsUBO);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(vec4), value_ptr(vec4(1.0, 1.0, 0, 1)));

    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    camera = new Camera( {0, 0, 50});
    camera->updateView();

    player = new Player();
    player->renderer = Renderer::create(Shader::create("shaders/ball.vert", "shaders/ball.frag"), createIcosphere());

    auto bubbleRenderer = Renderer::create(player->renderer->shader, player->renderer->mesh, 30);
    for (int i = 0; i < 30; ++i) {
        Bubble * bubble = new Bubble(i);
        bubble->renderer = bubbleRenderer;
    }

    aquarium = new Aquarium();
    aquarium->renderer = Renderer::create(Shader::create("shaders/ball.vert", "shaders/ball.frag"), player->renderer->mesh);

    renderers.push_back(aquarium->renderer);
    renderers.push_back(bubbleRenderer);
    renderers.push_back(player->renderer);

    glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);
    glfwSetKeyCallback(window, [] (GLFWwindow* window, int key, int scancode, int action, int mods) {
        if(key == GLFW_KEY_R && action == GLFW_PRESS){
            cerr << "Reloading...\n";
            for(auto renderer : GameController::instance->renderers)
                renderer->shader->reload();
        }
    });

    aquarium->renderer = Renderer::create(
            Shader::create("shaders/ball.vert", "shaders/ball.frag"),
            player->renderer->mesh);


}

void GameController::update(float dt) {
    glfwPollEvents();

    player->update(dt);
    for (Bubble *bubble : bubbles) {
        bubble->update(dt);
        if (player->collider * bubble->collider)
            CDBG << "collide\n";
    }
}

void GameController::render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    aquarium->renderer->render();
//    player->renderer->render();
//    for(auto renderer:renderers)
//        renderer->render();
}

#endif //GAMECONTROLLER
