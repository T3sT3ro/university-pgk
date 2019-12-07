//
// Created by tooster on 03.12.2019.
//

#ifndef ARKANOID_GAMECONTROLLER_HPP
#define ARKANOID_GAMECONTROLLER_HPP

#include "common.hpp"

#include <vector>
#include "GameObject.cpp"

constexpr int spheres = 50;

class Bubble : public GameObject {
public:
    explicit Bubble(GLuint instanceno);

    void update(float dt) override;
};

struct Camera {
    vec3  *position;
    vec3  forward = {0, 0, -1};
    vec3  up      = {0, 1, 0};
    float zoom    = 1; // todo
};

class Player : public GameObject {
public:
    Camera camera;

    Player();

    void update(float dt) override;
};

class GameController {
public:
    static GameController *instance;

    explicit GameController(GLFWwindow *window);

    void update(float dt);

    GLFWwindow *window;

    void render(Shader* shader, ShaderProperty* MBP);

private:
    Player         player;
    vector<Bubble> bubbles;
    Camera         *inside, outside;
};

GameController *GameController::instance;

#endif //ARKANOID_GAMECONTROLLER_HPP
