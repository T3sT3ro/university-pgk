//
// Created by tooster on 03.12.2019.
//

#ifndef ARKANOID_GAMECONTROLLER_HPP
#define ARKANOID_GAMECONTROLLER_HPP

#include "common.hpp"

#include <vector>
#include "GameObject.hpp"
#include "Shader.hpp"
#include "Camera.hpp"
#include "Renderer.hpp"

constexpr int spheres = 1;


class Bubble : public GameObject {
public:
    explicit Bubble(Renderer *renderer, GLuint instanceno);

    void update(float dt) override;
};




class Player : public GameObject {
public:
    Camera *camera;

    explicit Player(Renderer *renderer);

    void update(float dt) override;

private:
    constexpr static float angularSpeed = 1.5f;
    constexpr static float speed = 3.0f;
};




class Aquarium : public GameObject {
public:
    explicit Aquarium(Renderer* renderer);

    void update(float d) override;
};




class GameController {
public:
    static GameController *instance;
    GLFWwindow            *window;
    static Camera         *camera;

    explicit GameController(GLFWwindow *window);

    void update(float dt);

    void render();
    static GLuint matricesUBO;
    static GLuint lightsUBO;

private:
    Player             *player;
    vector<Bubble *>   bubbles;
    Aquarium           *aquarium;
    vector<Renderer *> renderQueue;
};

#endif //ARKANOID_GAMECONTROLLER_HPP
