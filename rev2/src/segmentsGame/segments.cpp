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
    float segmentLength;

public :

    std::vector<SegmentData> segments;
    int                      instances = 0;

    SegmentInstancedRenderer(int segmentCnt) : AGLDrawable(0) {
        segments.resize(segmentCnt);
        instances = segmentCnt;
        setShaders();
        setBuffers();
    }

    void setSegmentLength(float length) {
        segmentLength = length;
        setShaders();
        glUniform1f(0, segmentLength);
    }

    void setSegmentData(int segment, const SegmentData &data) {
        bindBuffers();
        segments[segment] = data;
        printf("%.2f %.2f\n", data.center.x, data.center.y);
        glBufferSubData(GL_ARRAY_BUFFER, sizeof(SegmentData) * segment, sizeof(SegmentData), &data);
    }

    void setShaders() {
        compileShadersFromFile("shaders/segment.vert", "shaders/segment.frag");
    }

    void setBuffers() {
        bindBuffers();
        glBufferData(GL_ARRAY_BUFFER, sizeof(SegmentData) * instances, segments.data(), GL_STATIC_DRAW);
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
        glDrawArraysInstanced(GL_LINES, 0, 2, instances);
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
    segments.setSegmentLength(1.0f/(float) n);

    srand(seed);

    // initialize segments with random rotation
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            segments.setSegmentData(i * n + j, {
                    glm::vec2(
                            glm::mix(-1.0f, 1.0f, (float) i / (float) (n-1)),
                            glm::mix(-1.0f, 1.0f, (float) j / (float) (n-1))
                    ),
                    ((float) rand() / (float) RAND_MAX) * 360.0f
            });
        }
    }
    segments.setSegmentData(0, {
        segments.segments[0].center,
        3.14/4.0
    });

    do {
        glClear(GL_COLOR_BUFFER_BIT);

        AGLErrors("main-loopbegin");
        // =====================================================    Drawing
        segments.draw();
        AGLErrors("main-afterdraw");

        glfwSwapBuffers(win()); // =============================    Swap buffers
        glfwPollEvents();
        //glfwWaitEvents();

        // =====================================================    Move player
        auto        currentPlayerParams = segments.segments[0];
        SegmentData newPlayerParams;

        glm::vec2 direction(cos(currentPlayerParams.rotation), sin(currentPlayerParams.rotation));
        float     motionMagnitude       = 0.0f;
        if (glfwGetKey(win(), GLFW_KEY_UP) == GLFW_PRESS) motionMagnitude += 0.01f;
        if (glfwGetKey(win(), GLFW_KEY_DOWN) == GLFW_PRESS) motionMagnitude -= 0.01f;

        float rotationMagnitude = 0.0f;
        if (glfwGetKey(win(), GLFW_KEY_LEFT) == GLFW_PRESS) rotationMagnitude += 0.05f;
        if (glfwGetKey(win(), GLFW_KEY_RIGHT) == GLFW_PRESS) rotationMagnitude -= 0.05f;


        // collision check
        newPlayerParams.center   = currentPlayerParams.center + direction * motionMagnitude;
        newPlayerParams.rotation = currentPlayerParams.rotation + rotationMagnitude;
        segments.setSegmentData(0, newPlayerParams);


    } while (

            glfwGetKey(win(), GLFW_KEY_ESCAPE) != GLFW_PRESS
            && glfwWindowShouldClose(win()) == 0
        // collided with last
            );
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