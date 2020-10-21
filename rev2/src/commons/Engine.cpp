//
// Created by tooster on 05.11.2019.
//

#ifndef GAMECONTROLLER
#define GAMECONTROLLER

#include "Engine.hpp"
#include "Shader.hpp"
#include "Renderer.hpp"
#include <iostream>
#include <cmath>

struct globals {
    mat4 viewMatrix;
    mat4 projectionMatrix;
    vec4 camPos;
    vec4 lightPos;
};

Engine *Engine::instance    = nullptr;
GLuint Engine::globalsUBO   = 0;
float  Engine::timescale    = 1.0;
float  Engine::maxFramerate = 60.0f;
bool   Engine::isRunning    = false;

Engine::Engine(GLFWwindow *window) : window(window) {
    instance = this;

    // magenta
    glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_DEPTH);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    // initializing global matrices UBO
    glGenBuffers(1, &globalsUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, globalsUBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(globals), nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, globalsUBO);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(mat4), value_ptr(mat4(1)));

    glBindBuffer(GL_UNIFORM_BUFFER, 0);

}

void Engine::processInput() {

}

void Engine::update(float dt) { dt *= timescale; }

void Engine::render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    for (auto renderer:renderQueue)
        renderer->render();
}

// game loop impl. from https://gafferongames.com/post/fix_your_timestep/
void Engine::start() {
    double t  = 0.0;
    double dt = 0.01;

    double lastTime = glfwGetTime();
    double accumulator = 0.0;

    while (isRunning) {

        double currentTime   = glfwGetTime();
        double frameTime = currentTime - lastTime;
        if (frameTime > 0.25) // ??? nani the fuck
            frameTime = 0.25;
        lastTime   = currentTime;

        accumulator += frameTime;

        while (accumulator >= dt) {
            update(dt);
            t += dt;
            accumulator -= dt;
        }

        const double alpha = accumulator / dt;

        update()
        State state = currentState * alpha +
                      previousState * (1.0 - alpha);

        render(alpha);
    }
}

void Engine::stop() {

}

#endif //GAMECONTROLLER
