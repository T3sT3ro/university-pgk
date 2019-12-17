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

struct Camera {
    vec3  *position = nullptr;
    vec3  forward = {0, 0, -1};
    vec3  up      = {0, 1, 0};
    float zoom    = 1; // todo
};



class Bubble : public GameObject {
public:
    explicit Bubble(GLuint instanceno);

    void update(float dt) override;
};



class Player : public GameObject {
public:
    Camera camera;

    Player();

    void update(float dt) override;

private:
    constexpr static float angularSpeed = 1.0f;
};




class GameController {
public:
    static GameController *instance;
    GLFWwindow            *window;

    explicit GameController(GLFWwindow *window);

    void update(float dt);

    void render();

private:
    Player *player;
    vector<Bubble *> bubbles;
    Camera            *inside, *outside;
    vector<Renderer*> renderers;
};

#endif //ARKANOID_GAMECONTROLLER_HPP
