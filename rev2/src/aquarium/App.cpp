//
// Created by tooster on 03.11.2019.
//

#include "common.hpp"
#include "Engine.hpp"

//Shader ballShader("shaders/ball.vert", "shaders/ball.frag");

void windowResizeHandler(GLFWwindow *, int, int);

class App {
    static GLFWwindow *window;
    static int width, height;
    static float lastframe;
public:
    Engine       *gc = nullptr;
    static float deltaTime;

    App(int width, int height) {
        App::width = width;
        App::height = height;
        if (!glfwInit()) CRITICAL("GLFW not initialized");
        glfwWindowHint(GLFW_SAMPLES, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);  // To make MacOS happy; should not be needed
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        window = glfwCreateWindow(width, height, "Aquarium", NULL, NULL);
        if (window == nullptr) CRITICAL("window not created");
        glfwMakeContextCurrent(window);
//        glewExperimental = true;
        if (glewInit() != GLEW_OK) CRITICAL("GLEW not initialized");

        gc = new Engine(window);

        glfwSetWindowSizeCallback(window, [](GLFWwindow *, int w, int h) {
            App::width = w;
            App::height = h;
            glViewport(0, 0, w, h);
        });
        lastframe = glfwGetTime();
    }

    void run() {
        do {
            double newframe = glfwGetTime();
            gc->update((App::deltaTime = newframe - lastframe));
            gc->render();
            lastframe = newframe;

            glfwSwapBuffers(window);
        } while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && !glfwWindowShouldClose(window));
    }

    virtual ~App() { delete gc; }
};

GLFWwindow* App::window;
int App::width, App::height;
float App::lastframe, App::deltaTime;

int main() {
    srand(time(nullptr));
    App app(1000, 800);
    atexit([]() { glfwTerminate(); cerr.flush();});
    app.run();
}

//