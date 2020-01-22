//
// Created by tooster on 05.11.2019.
//

#ifndef GAMECONTROLLER
#define GAMECONTROLLER

#include "GameController.hpp"
#include "Shader.hpp"
#include "Renderer.hpp"
#include <iostream>


Bubble::Bubble(Renderer *renderer, GLuint instanceno) : GameObject(renderer, instanceno) {
    transform.scale *= 3.0f + (rand() % 10) / 10.0f;
    velocity = vec3(0, (3.0f + (rand() % 10)), 0);
}

void Bubble::update(float dt) {
    transform.translation += velocity * speed * dt;
    if (transform.translation.z > 100) transform.translation.z = 0;

    renderer->setModelMatrix(transform.toModelMatrix(), instance);
    renderer->wireframe = true;
}




Player::Player(Renderer *renderer) : GameObject(renderer) {
    camera = new Camera(transform.translation);
    renderer->backfaceCulling = false;
    renderer->wireframe = true;
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
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        rot = rotate(rot, angularSpeed*dt, {0,1,0});
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        rot = rotate(rot, -angularSpeed*dt, {0,1,0});
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        rot = glm::rotate(rot, angularSpeed*dt, normalize(cross(camera->forward, camera->up)));
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        rot = glm::rotate(rot, -angularSpeed*dt, normalize(cross(camera->forward, camera->up)));

    camera->position = transform.translation - vec3(0,0,10);
    camera->forward = rot * vec4(camera->forward, 0);
    camera->up = rot * vec4(camera->up, 0);

    renderer->setModelMatrix(transform.toModelMatrix());
}




void Aquarium::update(float ) {}

Aquarium::Aquarium(Renderer *renderer) : GameObject(renderer) {
    transform.scale *= 10;
//    renderer->cullMode = GL_FRONT;
    renderer->backfaceCulling = false;
    renderer->wireframe = true;
}


GameController *GameController::instance;
GLuint GameController::matricesUBO;
GLuint GameController::lightsUBO;

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
    glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::vec4), nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, lightsUBO);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(vec4), value_ptr(vec4(1.0, 1.0, 0, 1)));

    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    Mesh* icosphere = Mesh::import("models/icosphere.obj");
    Mesh* suzanne = Mesh::import("models/suzanne.obj");
    Shader* ballShader = Shader::create("shaders/ball.vert", "shaders/ball.frag");

    player = new Player(Renderer::create(ballShader, suzanne));
    player->transform.translation = {0, 0, -10};
    camera = player->camera;


    auto bubbleRenderer = Renderer::create(ballShader, icosphere, spheres);
    for (int i = 0; i < spheres; ++i) bubbles.push_back(new Bubble(bubbleRenderer, i));


    aquarium = new Aquarium(Renderer::create(Shader::create("shaders/aquarium.vert", "shaders/aquarium.frag"), icosphere));
    aquarium->renderer->setModelMatrix(aquarium->transform.toModelMatrix());

    renderQueue.push_back(aquarium->renderer);
    renderQueue.push_back(player->renderer);
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
