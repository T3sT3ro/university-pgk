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

struct globals{
    mat4 viewMatrix;
    mat4 projectionMatrix;
    vec4 camPos;
    vec4 lightPos;
};

Player::Player() : GameObject(renderer) {
    camera = new Camera(transform.translation);
    speed = 100.0f;
    angularSpeed = 4.0f;
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
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)            rot = rotate(rot, angularSpeed*dt, {0,1,0});
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)           rot = rotate(rot, -angularSpeed*dt, {0,1,0});
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)              rot = glm::rotate(rot, angularSpeed*dt, normalize(cross(camera->forward, camera->up)));
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)            rot = glm::rotate(rot, -angularSpeed*dt, normalize(cross(camera->forward, camera->up)));
    if (glfwGetKey(window, GLFW_KEY_KP_ADD) == GLFW_PRESS)          fov = std::max(45.0f, fov-0.01f);
    if (glfwGetKey(window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS)     fov = std::min(100.0f, fov+0.01f);

    camera->fov = fov;
    camera->forward = rot * vec4(camera->forward, 0);
    camera->up = rot * vec4(camera->up, 0);
    camera->position = transform.translation - camera->forward*4.0f;

    renderer->setModelMatrix(transform.toModelMatrix());

}




void Earth::update(float ) {}

Earth::Earth(Renderer *renderer) : GameObject(renderer) {
    renderer->cullMode = GL_BACK;
    renderer->backfaceCulling = true;
    renderer->wireframe = true;

    renderer->render(GameController::camera);
}


GameController *GameController::instance;
GLuint GameController::globalsUBO;
Camera *GameController::camera;
float GameController::timescale = 1.0;

GameController::GameController(GLFWwindow *window) : window(window) {
    instance = this;

    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    glClearColor(.3f, .3f, .3f, .3f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_DEPTH);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable( GL_BLEND );

    // initializing global matrices UBO
    glGenBuffers(1, &globalsUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, globalsUBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(globals), nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, globalsUBO);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(mat4), value_ptr(mat4(1)));

    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    Mesh* icosphere = Mesh::import("models/icosphere.obj");
    Mesh* UVsphere = Mesh::import("models/UVsphere.obj");
    Shader* earthShader = Shader::create("shaders/earth.vert", "shaders/earth.frag");

    player = new Player();
    camera = player->camera;


    earth = new Earth(Renderer::create(earthShader, UVsphere));
    earth->renderer->setModelMatrix(earth->transform.toModelMatrix());

    renderQueue.push_back(earth->renderer);
    renderQueue.push_back(player->renderer);

    glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);
    glfwSetKeyCallback(window, [] (GLFWwindow* , int key, int , int action, int mods) {
        if(key == GLFW_KEY_R && action == GLFW_PRESS){
            if(mods & GLFW_MOD_CONTROL){
                cerr << "Reloading...\n";
                for(auto renderer : GameController::instance->renderQueue)
                    renderer->shader->reload();
            }
        }
    });
}

void GameController::update(float dt) {
    dt *= timescale;
    glfwPollEvents();
    player->update(dt);
}

void GameController::render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    for(auto renderer:renderQueue)
        renderer->render(camera);
}

#endif //GAMECONTROLLER
