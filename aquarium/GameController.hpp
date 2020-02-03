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

const float aquariumRadius = 25.0f;

class Bubble : public GameObject {
public:
    SphereCollider collider;

    explicit Bubble(Renderer *renderer, GLuint instanceno);

    void update(float dt) override;
};




class Player : public GameObject {
public:
    Camera *camera;
    SphereCollider collider;

    explicit Player(Renderer *renderer);

    void update(float dt) override;

private:
    float angularSpeed = 1.5f;
    float speed = 3.0f;
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
    static float          timescale;


    explicit GameController(GLFWwindow *window);

    void update(float dt);

    void render();
    static GLuint globalsUBO;

    void nextLevel();

private:
    Player             *player;
    vector<Bubble *>   bubbles;
    Aquarium           *aquarium;
    vector<Renderer *> renderQueue;
    Camera*            outCamera;
    int level = 0;
    int spheresDelta = 8;
};

#endif //ARKANOID_GAMECONTROLLER_HPP
