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
}


GameController *GameController::instance;
GLuint GameController::globalsUBO;
Camera *GameController::camera;
float GameController::timescale = 1.0;

Mesh* UVsphere;
Shader* earthShader;

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

    outCamera = new Camera({aquariumRadius*2, aquariumRadius, 0}, normalize(vec3(-1,-.5,0)));
    Mesh* icosphere = Mesh::import("models/icosphere.obj");
    UVsphere = Mesh::import("models/UVsphere.obj");
    //    Mesh* suzanne = Mesh::import("models/suzanne.obj");
    Mesh* box = Mesh::import("models/box.obj");

    player = new Player(Renderer::create(Shader::create("shaders/ball.vert", "shaders/ball.frag"), icosphere));
    camera = player->camera;


    bubbleShader = Shader::create("shaders/bubble.vert", "shaders/bubble.frag");

    aquarium = new Aquarium(Renderer::create(Shader::create("shaders/aquarium.vert", "shaders/aquarium.frag"), box));
    aquarium->renderer->setModelMatrix(aquarium->transform.toModelMatrix());

    renderQueue.push_back(aquarium->renderer);
    renderQueue.push_back(player->renderer);

    glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);
    glfwSetKeyCallback(window, [] (GLFWwindow* , int key, int , int action, int mods) {
        if(key == GLFW_KEY_R && action == GLFW_PRESS){
            if(mods & GLFW_MOD_CONTROL){
                cerr << "Reloading...\n";
                for(auto renderer : GameController::instance->renderQueue)
                    renderer->shader->reload();
            } else {
                GameController::instance->level=0;
                GameController::instance->nextLevel();
            }
        }
        if (key == GLFW_KEY_TAB && action == GLFW_PRESS){
            swap(GameController::camera, GameController::instance->outCamera);
            if(GameController::instance->player->camera == GameController::instance->camera){
                GameController::instance->aquarium->renderer->shader->setUniformFloat("FAR", GameController::camera->far);
                GameController::instance->bubbles[0]->renderer->shader->setUniformFloat("FAR", GameController::camera->far);
            } else {
                float INF = INFINITY;
                GameController::instance->aquarium->renderer->shader->setUniformFloat("FAR", INF);
                GameController::instance->bubbles[0]->renderer->shader->setUniformFloat("FAR", INF);
            }
        }
    });

    renderQueue.push_back(nullptr); // placeholder
    nextLevel();
}

void GameController::update(float dt) {
    dt *= timescale;

    glfwPollEvents();

    player->update(dt);
    for (Bubble *bubble : bubbles) {
        bubble->update(dt);
        if (player->collider * bubble->collider){
            cerr << "You Lost on level "<< level << " T-T press R to restart" << endl;
            timescale = 0;
        }
    }
}

void GameController::render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    for(auto renderer:renderQueue)
        renderer->render(camera);
}

void GameController::nextLevel() {
    ++level;
    cerr << "LEVEL " << level << endl;
    player->transform.translation = {0, 0, -aquariumRadius*2+4};

    timescale = 1;
    renderQueue.pop_back();
    bubbles.clear();
    auto bubbleRenderer = Renderer::create(bubbleShader, UVsphere, level*level*spheresDelta*spheresDelta);
    bubbleRenderer->shader->setUniformFloat("FAR", GameController::camera->far);
    for (int i = 0; i < level*level*spheresDelta*spheresDelta; ++i) bubbles.push_back(new Bubble(bubbleRenderer, i));
    renderQueue.push_back(bubbleRenderer);
}

#endif //GAMECONTROLLER
