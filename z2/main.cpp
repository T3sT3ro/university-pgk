//
// Created by tooster on 15.10.2019.
//

#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <common/shader.hpp>
#include "gameManager.cpp"

GLFWwindow *window;
GameManager gm;
GLuint      vertexbuffer;
GLuint      scorebuffer;
GLuint      bgProgID, linesProgID;
float       dt, animStart;
int         width = 800, height = 800;

bool doRestart = false;
struct restartOptions {
    MODE mode;
    int  maxMistakes;
} restartOpts {SEGMENT, 1};

void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    VERBOSE(fprintf(stderr, "KEY: %c %d %d %x\n", key, scancode, action, mods));
    if (action == GLFW_RELEASE && !doRestart) {
        switch (key) {
            case GLFW_KEY_5: restartOpts.maxMistakes = 5; break;
            case GLFW_KEY_4: restartOpts.maxMistakes = 4; break;
            case GLFW_KEY_3: restartOpts.maxMistakes = 3; break;
            case GLFW_KEY_2: restartOpts.maxMistakes = 2; break;
            case GLFW_KEY_1: restartOpts.maxMistakes = 1; break;
            case GLFW_KEY_O: restartOpts.mode = SEGMENT;  break;
            case GLFW_KEY_T: restartOpts.mode = TRIANGLE; break;
            restart:
            case GLFW_KEY_N:
                doRestart   = true;
                animStart   = dt;
        }
    }
}

void mouseCallback(GLFWwindow *window, int button, int action, int mods) {
    VERBOSE(fprintf(stderr, "MOUSE: %d %d %x\n", button, action, mods));
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE && !doRestart) {
        double x, y;
        int    w, h;
        glfwGetCursorPos(window, &x, &y);
        glfwGetWindowSize(window, &w, &h);
        glUniform1i(glGetUniformLocation(linesProgID, "lastFig"), gm.getFiguresCount());
        gm.putFigure({2.0 * x / w - 1.0, -2.0 * y / h + 1.0});

        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glBufferSubData(GL_ARRAY_BUFFER,
                        gm.getLastFigureIdx() * sizeof(vec2),
                        gm.getFigureSize() * sizeof(vec2),
                        &gm.endpoints[gm.getLastFigureIdx()]);

        glBindBuffer(GL_ARRAY_BUFFER, scorebuffer);
        vec2 progress[4] = {vec2(-.99, -1), vec2(-.99, -1 + 1.0 * gm.mistakes[0] / gm.maxMistakes),
                            vec2(+.99, -1), vec2(+.99, -1 + 1.0 * gm.mistakes[1] / gm.maxMistakes)};
        (fprintf(stderr, "MISSCNT: [%d : %d]\n", gm.mistakes[0], gm.mistakes[1]));
        glBufferData(GL_ARRAY_BUFFER, sizeof(progress), &progress, GL_STATIC_DRAW);
    }
}

void windowResizeHandler(GLFWwindow *window, int w, int h) {
    width  = w;
    height = h;
    glViewport(0, 0, width, height);
}

void init() {
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        getchar();
        exit(-1);
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);  // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    window = glfwCreateWindow(width, height, "Lines game", NULL, NULL);

    if (window == NULL) {
        fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
        getchar();
        glfwTerminate();
        exit(-1);
    }
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = true;  // Needed for core profile
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        getchar();
        glfwTerminate();
        exit(-1);
    }

    glfwMakeContextCurrent(window);
    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    glfwSetMouseButtonCallback(window, mouseCallback);
    glfwSetKeyCallback(window, keyCallback);
    glfwSetWindowSizeCallback(window, windowResizeHandler);

    gm.restart();
}

int main() {
    init();

    gm.resizeCallback = [](vec2 *data) {
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vec2) * gm.capacity, gm.endpoints, GL_DYNAMIC_DRAW);
    };

    glClearColor(0.13f, 0.13f, 0.13f, 1.0f);

    // BVO for bg
    linesProgID = LoadShaders("positional.vert", "lines.frag");
    bgProgID    = LoadShaders("positional.vert", "bg.frag");

    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);

    GLuint bgbuffer;
    glGenBuffers(1, &bgbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, bgbuffer);
    vec2 bgVertexArray[]{vec2(-1, -1), vec2(-1, 1), vec2(1, -1), vec2(1, 1)};
    glBufferData(GL_ARRAY_BUFFER, sizeof(bgVertexArray), bgVertexArray, GL_STATIC_DRAW);

    // VBO for lines
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec2) * gm.capacity, gm.endpoints, GL_DYNAMIC_DRAW);

    // VBO for progress bars
    vec2 scores[4] = {vec2(-.99, -1), vec2(-.99, -1), vec2(-.99, +1), vec2(-.99, +1)};
    glGenBuffers(2, &scorebuffer);
    glBindBuffer(GL_ARRAY_BUFFER, scorebuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(scores), scores, GL_STATIC_DRAW);

    fprintf(stderr, "<<%d %d>>\n", bgProgID, linesProgID);
    float animDuration = .5;
    bool  animShrink   = true;
    gm.restart();
    do {
        glClear(GL_COLOR_BUFFER_BIT);

        // animate shrink
        dt = glfwGetTime();
        if (doRestart) {  // animation
            float fracRaw = (dt - animStart) / animDuration;
            float frac    = glm::smoothstep(0.0f, 1.0f, fracRaw);
            if (dt - animStart < animDuration && animShrink) {  // shrink viewport
                glViewport(glm::mix(0, width / 2, frac), glm::mix(0, height / 2, frac), glm::mix(width, 0, frac), glm::mix(width, 0, frac));
                glUniform1f(glGetUniformLocation(linesProgID, "desaturateFactor"), fracRaw);
            } else if (animShrink) {  // do restart while viewport is 0
                animShrink     = false;
                animStart      = dt;  // start explode animation
                gm.maxMistakes = restartOpts.maxMistakes;
                gm.mode        = restartOpts.mode;
                gm.restart();
            } else if (dt - animStart < animDuration) {  // explode anim
                glViewport(glm::mix(0, width / 2, 1 - frac), glm::mix(0, height / 2, 1 - frac), glm::mix(width, 0, 1 - frac), glm::mix(width, 0, 1 - frac));
                glUniform1f(glGetUniformLocation(linesProgID, "desaturateFactor"), 1 - fracRaw);
            } else {  // after explode
                doRestart  = false;
                animShrink = true;
                glViewport(0, 0, width, height);
                glUniform1f(glGetUniformLocation(linesProgID, "desaturateFactor"), 0);
            }
        }

        // draw background
        glUseProgram(bgProgID);
        glUniform1f(glGetUniformLocation(bgProgID, "dt"), dt);
        glBindVertexArray(VertexArrayID);
        glBindBuffer(GL_ARRAY_BUFFER, bgbuffer);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
        if (gm.mode == TRIANGLE) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glDisableVertexAttribArray(0);

        // draw board
        glUseProgram(linesProgID);
        glBindVertexArray(VertexArrayID);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
        glDrawArrays(gm.mode == SEGMENT ? GL_LINES : GL_TRIANGLES, 0, gm.size);
        glDisableVertexAttribArray(0);

        // draw status bars
        glEnableVertexAttribArray(0);
        glLineWidth(6.0);
        glBindBuffer(GL_ARRAY_BUFFER, scorebuffer);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
        glDrawArrays(GL_LINES, 0, 4);
        glDisableVertexAttribArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    } while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
             glfwWindowShouldClose(window) == 0);

    glDeleteBuffers(1, &scorebuffer);
    glDeleteBuffers(1, &vertexbuffer);
    glDeleteBuffers(1, &bgbuffer);

    glDeleteVertexArrays(1, &VertexArrayID);
    glDeleteProgram(linesProgID);
    glDeleteProgram(bgProgID);

    glfwTerminate();
    fflush(stderr);
    return 0;
}