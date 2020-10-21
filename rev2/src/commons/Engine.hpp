//
// Created by tooster on 03.12.2019.
//

#ifndef ENGINE_HPP
#define ENGINE_HPP

#include "common.hpp"

#include <vector>
#include "GameObject.hpp"
#include "Shader.hpp"
#include "Camera.hpp"
#include "Renderer.hpp"

class Engine {
public:
    static Engine *instance;
    GLFWwindow    *window;
    static float  timescale;
    static float  maxFramerate;

    explicit Engine(GLFWwindow *window);

    // starts the input-update-render loop
    void start();
    void stop();

    // uniform object -- get rid in terms of Renderer static globals
    static GLuint globalsUBO;

private:

    void processInput();
    void update(float dt);
    void render();
    vector<Renderer *> renderQueue;
    static bool isRunning;
};

#endif //ENGINE_HPP
