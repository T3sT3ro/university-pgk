//
// Created by tooster on 03.12.2019.
//

#ifndef ARKANOID_GAMECONTROLLER_HPP
#define ARKANOID_GAMECONTROLLER_HPP

#include "common.hpp"

#include <vector>
#include "GameObject.cpp"
#include "Shader.hpp"

constexpr int spheres = 50;

/// every modification to
class Camera {
public:
    // view data
    vec3  position;
    vec3  forward;
    vec3  up;
    // projection data
    float fov    = 75.0f; // todo
    float aspect = 4.0f/3.0f;
    float near = 1.0f;
    float far = 100.0f;

    Camera(vec3 position = {0,0,0}, vec3 forward = {0,0,-1}, vec3 up = {0,1,0});

    /// updates global view matrix
    void updateView();

    /// updates global projection matrix
    void updateProjection();
};




class Bubble : public GameObject {
public:
    explicit Bubble(GLuint instanceno);

    void update(float dt) override;
};




class Player : public GameObject {
public:
    Camera *camera;

    Player();

    void update(float dt) override;

private:
    constexpr static float angularSpeed = 1.0f;
};




class Aquarium : public GameObject {
public:
    Aquarium();
};




class GameController {
public:
    static GameController *instance;
    GLFWwindow            *window;

    explicit GameController(GLFWwindow *window);

    void update(float dt);

    void render();
    static GLuint matricesUBO;
    static GLuint lightsUBO;

private:
    Player             *player;
    vector<Bubble *>   bubbles;
    Aquarium           *aquarium;
    Camera             *camera;
    vector<Renderer *> renderers;
};

#endif //ARKANOID_GAMECONTROLLER_HPP
