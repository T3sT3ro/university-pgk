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

using namespace std;
using namespace glm;

Mesh* flat;
Shader* flatShader;
Renderer* flatRenderer;

GameController *GameController::instance;

GLuint vao, vbo, indices;
vec3 v[4] {{-1,-1,0}, {1,-1,0}, {1,1,0}, {-.5f, .5f, 0}};
GLushort i[6] {0, 2, 1, 0, 2, 3};





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
    camera.position = &transform.translation;
}

void Player::update(float dt) {
    velocity = vec3(0);
    GLFWwindow *window = GameController::instance->window;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) velocity += camera.forward;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) velocity -= camera.forward;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) velocity += cross(camera.forward, camera.up);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) velocity -= cross(camera.forward, camera.up);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) velocity += camera.up;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) velocity -= camera.up;

    transform.translation += velocity * dt * speed;
    mat4 rot = glm::identity<mat4>();
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        rot = glm::rotate(rot, angularSpeed*dt, {0,1,0});
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        rot = glm::rotate(rot, -angularSpeed*dt, {0,1,0});
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        rot = glm::rotate(rot, angularSpeed*dt, glm::cross(camera.forward, camera.up));
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        rot = glm::rotate(rot, -angularSpeed*dt, glm::cross(camera.forward, camera.up));
    camera.forward = rot * vec4(camera.forward, 0);
    camera.up = rot * vec4(camera.up, 0);
}




GameController::GameController(GLFWwindow *window) : window(window) {
    instance = this;

    player = new Player();
    player->renderer = Renderer::create(Shader::create("shaders/ball.vert", "shaders/ball.frag"), createIcosphere());
    auto bubbleRenderer = Renderer::create(player->renderer->shader, player->renderer->mesh, 30);

    renderers.push_back(player->renderer);
    renderers.push_back(bubbleRenderer);

    for (int i = 0; i < 30; ++i) {
        Bubble * bubble = new Bubble(i);
        bubble->renderer = bubbleRenderer;
    }

    glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);
    glfwSetKeyCallback(window, [] (GLFWwindow* window, int key, int scancode, int action, int mods) {
        if(key == GLFW_KEY_R && action == GLFW_PRESS){
            cerr << "Reloading...\n";
            for(auto renderer : GameController::instance->renderers)
                renderer->shader->reload();
        }
    });

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &indices);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices);

    glBufferData(GL_ARRAY_BUFFER, sizeof(v), v, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(i), i, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, sizeof(vec3)/ sizeof(GL_FLOAT), GL_FLOAT, GL_FALSE, 0, nullptr);
    glBindVertexArray(0);

    vector<vec3> vv(v, v+ sizeof(v)/sizeof(vec3));
    vector<GLushort> iv(i, i+ sizeof(i)/sizeof(GLushort));

    flat = Mesh::create(vv, iv);
    flatShader = Shader::create("shaders/ball.vert", "shaders/ball.frag");
    flatRenderer = Renderer::create(flatShader, flat);
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
    glClear(GL_COLOR_BUFFER_BIT);
//    player->renderer->render();
//    for(auto renderer:renderers)
//        renderer->render();
//    glBindVertexArray(flatRenderer->VAO);
    glBindVertexArray(vao);
    glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr, 2);
    flatRenderer->render();
}

#endif
