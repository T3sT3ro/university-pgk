//
// Created by tooster on 05.11.2019.
//

#ifndef GAMECONTROLLER
#define GAMECONTROLLER

#include "GameController.hpp"
#include "Shader.hpp"
#include "Renderer.hpp"
#include <iostream>
#include <cmath>


Bubble::Bubble(Renderer *renderer, GLuint instanceno) : GameObject(renderer, instanceno) {
    transform.scale *= .5f + (rand() % 10) / 10.0f;
    velocity = vec3(0, (3.0f + (rand() % 10)), 0);
    transform.translation.x = instanceno;
}

void Bubble::update(float dt) {
    transform.translation += velocity * speed * dt;
    if (transform.translation.z > 100) transform.translation.z = 0;

    renderer->setModelMatrix(transform.toModelMatrix(), instance);
    renderer->wireframe = true;
}




Player::Player(Renderer *renderer) : GameObject(renderer) {
    camera = new Camera(transform.translation);
    renderer->backfaceCulling = true;
    renderer->wireframe = false;
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

    transform.translation += velocity * dt * speed;
    mat4 rot = mat4(1.0f);
    float fov = camera->fov;
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        rot = rotate(rot, angularSpeed*dt, {0,1,0});
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        rot = rotate(rot, -angularSpeed*dt, {0,1,0});
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        rot = glm::rotate(rot, angularSpeed*dt, normalize(cross(camera->forward, camera->up)));
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        rot = glm::rotate(rot, -angularSpeed*dt, normalize(cross(camera->forward, camera->up)));
    if (glfwGetKey(window, GLFW_KEY_KP_ADD) == GLFW_PRESS)
        fov = std::max(45.0f, fov-0.01f);
    if (glfwGetKey(window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS)
        fov = std::min(100.0f, fov+0.01f);

    camera->fov = fov;
    camera->forward = rot * vec4(camera->forward, 0);
    camera->up = rot * vec4(camera->up, 0);
    camera->position = transform.translation - camera->forward*4.0f + camera->up;

    renderer->setModelMatrix(transform.toModelMatrix());

    glBindBuffer(GL_UNIFORM_BUFFER, GameController::lightsUBO);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(vec4), value_ptr(vec4(transform.translation, 1)));
}




void Aquarium::update(float ) {}

Aquarium::Aquarium(Renderer *renderer) : GameObject(renderer) {
    transform.scale *= 5;
    renderer->cullMode = GL_FRONT;
    renderer->backfaceCulling = true;
    renderer->wireframe = false;
    renderer->shader->setUniformFloat("FAR", GameController::camera->far);
}


GameController *GameController::instance;
GLuint GameController::matricesUBO;
GLuint GameController::lightsUBO;
Camera *GameController::camera;

GameController::GameController(GLFWwindow *window) : window(window) {
    instance = this;

    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    glClearColor(.3f, .3f, .3f, .3f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_DEPTH);

    // initializing global matrices UBO
    glGenBuffers(1, &matricesUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, matricesUBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(mat4)*2, nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, matricesUBO);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(mat4), value_ptr(mat4(1)));

    // initializing global lights UBO
    glGenBuffers(1, &lightsUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, lightsUBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::vec3), nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, lightsUBO);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(vec4), value_ptr(vec4(0, 0, 0, 0)));

    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    Mesh* icosphere = Mesh::import("models/icosphere.obj");
//    Mesh* suzanne = Mesh::import("models/suzanne.obj");
    Shader* ballShader = Shader::create("shaders/ball.vert", "shaders/ball.frag");

    player = new Player(Renderer::create(ballShader, icosphere));
    player->transform.translation = {0, 0, -10};
    camera = player->camera;


    auto bubbleRenderer = Renderer::create(ballShader, icosphere, spheres);
    for (int i = 0; i < spheres; ++i) bubbles.push_back(new Bubble(bubbleRenderer, i));


    aquarium = new Aquarium(Renderer::create(Shader::create("shaders/aquarium.vert", "shaders/aquarium.frag"), icosphere));
    aquarium->renderer->setModelMatrix(aquarium->transform.toModelMatrix());
    aquarium->renderer->texture = Texture::import("textures/normal.jpg");

    renderQueue.push_back(aquarium->renderer);
//    renderQueue.push_back(player->renderer);
//    renderQueue.push_back(bubbleRenderer);

    glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);
    glfwSetKeyCallback(window, [] (GLFWwindow* , int key, int , int action, int ) {
        if(key == GLFW_KEY_R && action == GLFW_PRESS){
            cerr << "Reloading...\n";
            for(auto renderer : GameController::instance->renderQueue)
                renderer->shader->reload();
        }
    });

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
    for(auto renderer:renderQueue)
        renderer->render(camera);
}

#endif //GAMECONTROLLER
