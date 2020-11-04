//
// Created by tooster on 30.10.2020.
//
#include <vector>

#include <AGL3Window.hpp>
#include <AGL3Drawable.hpp>
#include <glm/glm.hpp>
#include <intersection.hpp>

struct SegmentData {
    glm::vec2 center;
    float     rotation;
};

class SegmentInstancedRenderer : public AGLDrawable {
    float extendLength;

public :

    std::vector<SegmentData> instances;
    int                      instancesCnt = 0;

    SegmentInstancedRenderer(int segmentCnt) : AGLDrawable(0) {
        instances.resize(segmentCnt);
        instancesCnt = segmentCnt;
        setShaders();
        setBuffers();
    }

    void setExtentLength(float length) {
        extendLength = length;
        setShaders();
        glUniform1f(0, extendLength);
    }

    float getSegmentLength() const { return extendLength; }

    void setSegmentData(int segment, const SegmentData &data) {
        bindBuffers();
        instances[segment] = data;
        glBufferSubData(GL_ARRAY_BUFFER, sizeof(SegmentData) * segment, sizeof(SegmentData), &data);
    }

    void setShaders() {
        compileShadersFromFile("shaders/segment.vert", "shaders/segment.frag");
    }

    void setBuffers() {
        bindBuffers();
        glBufferData(GL_ARRAY_BUFFER, sizeof(SegmentData) * instancesCnt, instances.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(SegmentData), (void *) offsetof(SegmentData, center));
        glVertexAttribDivisor(0, 1);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(SegmentData), (void *) offsetof(SegmentData, rotation));
        glVertexAttribDivisor(1, 1);
    }

    void draw() {
        bindProgram();
        bindBuffers();
        glDrawArraysInstanced(GL_LINES, 0, 2, instancesCnt);
    }

};

// ==========================================================================
// Window Main Loop Inits ...................................................
// ==========================================================================
class MyWin : public AGLWindow {
public:
    MyWin() {};

    MyWin(int _wd, int _ht, const char *name, int vers, int fullscr = 0)
            : AGLWindow(_wd, _ht, name, vers, fullscr) {};

    virtual void KeyCB(int key, int scancode, int action, int mods);

    void MainLoop(int seed, int n);

    void Resize(int _wd, int _ht) override;
};


// ==========================================================================
void MyWin::KeyCB(int key, int scancode, int action, int mods) {
    AGLWindow::KeyCB(key, scancode, action, mods); // f-key full screen switch
    if ((key == GLFW_KEY_SPACE) && action == GLFW_PRESS) { ; // do something
    }
    if (key == GLFW_KEY_HOME && (action == GLFW_PRESS)) { ; // do something
    }
}


// ==========================================================================
void MyWin::MainLoop(int seed, int n) {
    ViewportOne(0, 0, wd, ht);

    glLineWidth(2.0f);

    SegmentInstancedRenderer segments(n * n);
    segments.setExtentLength(1.0f / (float) n); // 1 = 2/2 - viewport size in NDC is 2x2

    srand(seed);

    // initialize segments with random rotation
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            segments.setSegmentData(i * n + j, {
                    glm::vec2(
                            glm::mix(-.9f, .9f, (float) i / (float) (n - 1)),
                            glm::mix(-.9f, .9f, (float) j / (float) (n - 1))
                    ),
                    ((float) rand() / (float) RAND_MAX) * 360.0f,
            });
        }
    }
    // rotate player 45 degrees from OX

    segments.setSegmentData(0, {
            segments.instances[0].center,
            M_PI / 4.0,
    });

    bool hasWon = false;

    double timerStart = glfwGetTime();
    double currentFrame = glfwGetTime();
    do {
        double deltaTime = glfwGetTime() - currentFrame;
        currentFrame = glfwGetTime();

        glClear(GL_COLOR_BUFFER_BIT);

        AGLErrors("main-loopbegin");
        // =====================================================    Drawing
        segments.draw();
        AGLErrors("main-afterdraw");

        glfwSwapBuffers(win()); // =============================    Swap buffers
        glfwPollEvents();
        //glfwWaitEvents();

        // =====================================================    Move player
        auto currentPlayerParams  = segments.instances[0];

        glm::vec2 direction(cos(currentPlayerParams.rotation), sin(currentPlayerParams.rotation));
        float     motionMagnitude = 0.0f;
        if (glfwGetKey(win(), GLFW_KEY_UP) == GLFW_PRESS) motionMagnitude += 1.0f;
        if (glfwGetKey(win(), GLFW_KEY_DOWN) == GLFW_PRESS) motionMagnitude -= 1.0f;

        float rotationMagnitude = 0.0f;
        if (glfwGetKey(win(), GLFW_KEY_LEFT) == GLFW_PRESS) rotationMagnitude += 1.0f;
        if (glfwGetKey(win(), GLFW_KEY_RIGHT) == GLFW_PRESS) rotationMagnitude -= 1.0f;


        // =====================================================    Collision check
        SegmentData newPlayerParams = {
                glm::clamp(currentPlayerParams.center + direction * motionMagnitude * (float) deltaTime * .5f,
                           glm::vec2(-0.9, -0.9),
                           glm::vec2(0.9, 0.9)),
                currentPlayerParams.rotation + rotationMagnitude * (float) deltaTime * 2.0f,
        };

        int other = n * n;
        while (--other) { // check collision from last to 0 (excluding 0)
            auto  playerExtent = glm::vec2(cos(newPlayerParams.rotation), sin(newPlayerParams.rotation));
            auto  otherExtent  = glm::vec2(cos(segments.instances[other].rotation), sin(segments.instances[other].rotation));
            float extentLength = segments.getSegmentLength();

            if (intersectSegmentSegment(
                    newPlayerParams.center + playerExtent * extentLength,
                    newPlayerParams.center - playerExtent * extentLength,
                    segments.instances[other].center + otherExtent * extentLength,
                    segments.instances[other].center - otherExtent * extentLength))
                break;
        }

        if (other == n * n - 1) { // collided with top last
            printf("Completed in %f\n", glfwGetTime() - timerStart);
            hasWon = true;
        } else if (other == 0) { // move if not colliding
            segments.setSegmentData(0, newPlayerParams);
        }


    } while (

            glfwGetKey(win(), GLFW_KEY_ESCAPE) != GLFW_PRESS
            && glfwWindowShouldClose(win()) == 0
            && !hasWon
            );
}

void MyWin::Resize(int _wd, int _ht) {
    AGLWindow::Resize(_wd, _ht);

    float edge = std::min(_wd, _ht);
    ViewportOne((_wd - edge)/2, (_ht - edge)/2, edge, edge);
}

int main(int argc, char *argv[]) {
    MyWin win;
    int   seed = time(nullptr);
    int   n    = 10;
    if (argc > 1) seed = atoi(argv[1]);
    if (argc > 2) n    = atoi(argv[2]);
    win.Init(600, 600, "segments", 0, 33);
    win.MainLoop(seed, n);
    return 0;
}