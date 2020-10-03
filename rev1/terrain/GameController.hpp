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

class Player : public GameObject {
public:
    Camera *camera;

    explicit Player();

    void update(float dt) override;

    float latitude = 0;
    float longitude = 0;
private:
    float angularSpeed;
    float speed;
};




class Earth : public GameObject {
public:
    explicit Earth(Renderer* renderer);

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
    Earth              *earth;
    vector<Renderer *> renderQueue;
    Camera*            outCamera;
};

#endif //ARKANOID_GAMECONTROLLER_HPP
