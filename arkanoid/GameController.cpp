//
// Created by tooster on 05.11.2019.
//

#ifndef GAMECONTROLLER
#define GAMECONTROLLER

#include "common.hpp"

#include <vector>

using namespace std;
using namespace glm;

class GameController {
public:
    class GameControllerState {
    public:
        virtual void onEnter();
        virtual GameControllerState* update(double dt); // if return is not null, transition occurs
        GameController *gc;
        GameControllerState(GameController *gc) : gc(gc) {};
    };

    GameController(GLFWwindow *window) : window(window) {
        glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);
        currentState = new ReadyState(this);
        currentState->onEnter();
    }

    void update(double dt) {
        auto newState = currentState->update(dt);
        if(newState != nullptr){
            delete currentState;
            currentState = newState;
            newState->onEnter();
        }
    }

    class ReadyState : public GameControllerState {
    public:
        ReadyState(GameController *pController) : GameControllerState(pController) {}

    private:
        void onEnter() override {
            // reset pad pos and velocity
            // reset ball and velocity
        }

        GameControllerState* update(double dt) override {
            glfwPollEvents();
            if(glfwGetKey(gc->window, GLFW_KEY_SPACE) != GLFW_PRESS)
                return new PlayingState(gc);
            return nullptr;
        }
    };

    class PlayingState : public GameControllerState {
    public:
        PlayingState(GameController *gc) : GameControllerState(gc) {}

    };

private:
    GLFWwindow *window;
    GameControllerState *currentState;

};

#endif