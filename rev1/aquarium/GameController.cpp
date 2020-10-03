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

Bubble::Bubble(Renderer *renderer, GLuint instanceno) : GameObject(renderer, instanceno), collider(this) {
    transform.scale *= .2f + random() % 10 / 10.0f;
    collider.radius = 1;
    velocity = vec3(0, (2.0f + random() / (float)RAND_MAX * 100.0), 0);
    speed = .05f;
//    double a = (float)random()/RAND_MAX * 2 * 3.1415;
//    double r = aquariumRadius * sqrt((float)random()/RAND_MAX);

    transform.translation.x = (2*(random()%(int)aquariumRadius) - aquariumRadius)*0.95;
    transform.translation.z = (4*(random()%(int)aquariumRadius) - 2*aquariumRadius)*0.95;
    transform.translation.y = -aquariumRadius/2 + 3; // tp to bottom of sphere
}

void Bubble::update(float dt) {
    transform.translation += velocity * speed * dt;
    float y=transform.translation.y;
    if (y > aquariumRadius/2){
        transform.translation.x = (2*(random()%(int)aquariumRadius) - aquariumRadius)*0.95;
        transform.translation.z = (4*(random()%(int)aquariumRadius) - 2*aquariumRadius)*0.95;
        transform.translation.y = -aquariumRadius/2; // tp to bottom of sphere
    }
    renderer->setModelMatrix(transform.toModelMatrix(), instance);
    renderer->wireframe = true;
    renderer->backfaceCulling = false;
}




Player::Player(Renderer *renderer) : GameObject(renderer), collider(this) {
    transform.scale *= 0.2;
    collider.radius = 1;
    camera = new Camera(transform.translation);
    renderer->backfaceCulling = false;
    renderer->wireframe = true;
    speed = 3.5f;
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
    transform.translation.x = clamp(transform.translation.x, -aquariumRadius+.5f, +aquariumRadius-.5f);
    transform.translation.y = clamp(transform.translation.y, -aquariumRadius/2+.5f, +aquariumRadius/2-.5f);
    transform.translation.z = clamp(transform.translation.z, -aquariumRadius*2+.5f, +aquariumRadius*2+.5f);

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
    camera->position = transform.translation - camera->forward*4.0f;

    renderer->setModelMatrix(transform.toModelMatrix());

    glBindBuffer(GL_UNIFORM_BUFFER, GameController::globalsUBO);
    glBufferSubData(GL_UNIFORM_BUFFER, offsetof(globals, lightPos), sizeof(vec4), value_ptr(vec4(transform.translation, 1)));

    if(transform.translation.z > aquariumRadius*2) GameController::instance->nextLevel();
}




void Aquarium::update(float ) {}

Aquarium::Aquarium(Renderer *renderer) : GameObject(renderer) {
    transform.scale *= aquariumRadius;
    transform.rotation.y = 90;
    renderer->cullMode = GL_BACK;
    renderer->backfaceCulling = true;
    renderer->wireframe = false;
    renderer->shader->setUniformFloat("FAR", GameController::camera->far);
}


GameController *GameController::instance;
GLuint GameController::globalsUBO;
Camera *GameController::camera;
float GameController::timescale = 1.0;

Mesh* UVsphere;
Shader* bubbleShader;

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
