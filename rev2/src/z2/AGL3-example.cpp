// ==========================================================================
// AGL3:  GL/GLFW init AGLWindow and AGLDrawable class definitions
//
// Ver.3  14.I.2020 (c) A. Łukaszewski
// Ver.4  21.X.2020 Modified by M. Polarczyk AKA Tooster
// ==========================================================================
// AGL3 example usage 
//===========================================================================
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include <AGL3Window.hpp>
#include <AGL3Drawable.hpp>

#define PI_TWO            1.570796326794897
#define PI                3.141592653589793
#define TWO_PI            6.283185307179586
#define CIRCLE_SCALE      1.0/16.0

// ==========================================================================
// Drawable object: no-data only: vertex/fragment programs
// ==========================================================================
class MyTri : public AGLDrawable {
public:
    MyTri() : AGLDrawable(0) {
        setShaders();
    }

    void setShaders() {
        compileShaders(R"END(

        #version 330
        #extension GL_ARB_explicit_uniform_location : require
        #extension GL_ARB_shading_language_420pack : require
        out vec4 vcolor;
        // output not needed - using gl_FragCoord instead
        void main(void) {
            const vec2 vertices[3] = vec2[3](vec2( 0.9, -0.9),
                                             vec2(-0.9, -0.9),
                                             vec2( 0.9,  0.9));
            const vec4 colors[]    = vec4[3](vec4(1.0, 0.0, 0.0, 1.0),
                                             vec4(0.0, 1.0, 0.0, 1.0),
                                             vec4(0.0, 0.0, 1.0, 1.0));
            vcolor      = colors[gl_VertexID];

            gl_Position = vec4(vertices[gl_VertexID], 0.5, 1.0);
         }

      )END", R"END(

         #version 330 
         #extension GL_ARB_explicit_uniform_location : require
         // viewport taken from uniform
         layout(location = 0) uniform vec2 viewportSize;
         in  vec4 vcolor;
         out vec4 color;

         void main(void) {
            if(distance(gl_FragCoord.xy/viewportSize.xy, vec2(0.5, 0.5)) < 0.2) {
                color = vec4(1.0, 1.0, 1.0, 1.0);
            } else {
                color = vcolor;
            }
         } 

      )END");
    }

    void draw(int wd, int ht) {
        bindProgram();
        glUniform2f(0, wd, ht);  // center in vertex shader
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }
};


// ==========================================================================
// Drawable object with some data in buffer and vertex/fragment programs
// ==========================================================================
class MyCross : public AGLDrawable {
public:
    MyCross() : AGLDrawable(0) {
        setShaders();
        setBuffers();
    }

    void setShaders() {
        compileShaders(R"END(

         #version 330 
         #extension GL_ARB_explicit_uniform_location : require
         #extension GL_ARB_shading_language_420pack : require
         layout(location = 0) in vec2 pos;
         layout(location = 0) uniform float scale;
         layout(location = 1) uniform vec2  center;
         out vec4 vtex;

         void main(void) {
            vec2 p = (pos * scale + center);
            gl_Position = vec4(p, 0.0, 1.0);
         }

      )END", R"END(

         #version 330 
         #extension GL_ARB_explicit_uniform_location : require
         layout(location = 3) uniform vec3  cross_color;
         out vec4 color;

         void main(void) {
            color = vec4(cross_color,1.0);
         } 

      )END");
    }

    void setBuffers() {
        bindBuffers();
        GLfloat vert[4][2] = {  // Cross lines
                {-1, 0},
                {1,  0},
                {0,  -1},
                {0,  1}
        };

        glBufferData(GL_ARRAY_BUFFER, 4 * 2 * sizeof(float), vert, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(
                0,                 // attribute 0, must match the layout in the shader.
                2,                  // size
                GL_FLOAT,           // type
                GL_FALSE,           // normalized?
                0,//24,             // stride
                (void *) 0            // array buffer offset
        );
    }

    void draw(float tx, float ty) {
        bindProgram();
        bindBuffers();
        glUniform1f(0, 1.0 / 16);  // scale  in vertex shader
        glUniform2f(1, tx, ty);  // center in vertex shader
        glUniform3f(3, cross_color[0], cross_color[1], cross_color[2]);

        glDrawArrays(GL_LINES, 0, 4);
    }

    void setColor(float r, float g, float b) {
        cross_color[0] = r;
        cross_color[1] = g;
        cross_color[2] = b;
    }

private:
    GLfloat cross_color[3] = {0.0, 1.0, 0.0};
};


class MyCircle : public AGLDrawable {
public:
    MyCircle() : AGLDrawable(0) {
        setShaders();
        setBuffers();
    }

    void setShaders() {
        compileShaders(R"END(

         #version 330
         #extension GL_ARB_explicit_uniform_location : require
         #extension GL_ARB_shading_language_420pack : require
         layout(location = 0) in vec2 pos;
         layout(location = 0) uniform float scale;
         layout(location = 1) uniform vec2  center;
         out vec4 vtex;

         void main(void) {
            vec2 p = (pos * scale + center);
            gl_Position = vec4(p, 0.0, 1.0);
         }

      )END", R"END(

         #version 330
         #extension GL_ARB_explicit_uniform_location : require
         out vec4 color;

         void main(void) {
            color = vec4(1.0, 0.0, 1.0 ,1.0);
         }

      )END");
    }

    static const int SEGMENTS = 32;

    void setBuffers() {
        bindBuffers();
        GLfloat  vert[SEGMENTS][2];
        for (int i = 0; i < SEGMENTS; ++i) {
            vert[i][0] = cos(i * TWO_PI / SEGMENTS);
            vert[i][1] = sin(i * TWO_PI / SEGMENTS);
        }

        glBufferData(GL_ARRAY_BUFFER, SEGMENTS * 2 * sizeof(float), vert, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(
                0,                 // attribute 0, must match the layout in the shader.
                2,                  // size
                GL_FLOAT,           // type
                GL_FALSE,           // normalized?
                0,//24,             // stride
                (void *) 0            // array buffer offset
        );
    }

    void draw(float cx, float cy) {
        bindProgram();
        bindBuffers();
        glUniform1f(0, CIRCLE_SCALE);  // scale  in vertex shader
        glUniform2f(1, cx, cy);  // center in vertex shader

        glDrawArrays(GL_TRIANGLE_FAN, 0, SEGMENTS);
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

    void MainLoop();
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
void MyWin::MainLoop() {
    ViewportOne(0, 0, wd, ht);

    MyCross  cross;
    MyTri    trian;
    MyCircle circle;

    float tx = 0.0, ty = 0.5;
    float cx = 0.0, cy = 0.0;
    float dx = (cx - tx);
    float dy = (cy - ty);

    do {
        glClear(GL_COLOR_BUFFER_BIT);

        AGLErrors("main-loopbegin");
        // =====================================================        Drawing
        trian.draw(this->wd, this->ht);
        cross.draw(tx, ty);
        circle.draw(cx, cy);
        AGLErrors("main-afterdraw");

        glfwSwapBuffers(win()); // =============================   Swap buffers
        glfwPollEvents();
        //glfwWaitEvents();

        if (glfwGetKey(win(), GLFW_KEY_DOWN) == GLFW_PRESS) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        } else if (glfwGetKey(win(), GLFW_KEY_UP) == GLFW_PRESS) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
        if (glfwGetKey(win(), GLFW_KEY_RIGHT) == GLFW_PRESS) tx += 0.01;
        if (glfwGetKey(win(), GLFW_KEY_LEFT) == GLFW_PRESS) tx -= 0.01;

        if (glfwGetKey(win(), GLFW_KEY_W) == GLFW_PRESS) cy += 0.01;
        if (glfwGetKey(win(), GLFW_KEY_S) == GLFW_PRESS) cy -= 0.01;
        if (glfwGetKey(win(), GLFW_KEY_A) == GLFW_PRESS) cx -= 0.01;
        if (glfwGetKey(win(), GLFW_KEY_D) == GLFW_PRESS) cx += 0.01;

        dx = (cx - tx);
        dy = (cy - ty);
    } while (

            glfwGetKey(win(), GLFW_KEY_ESCAPE) != GLFW_PRESS
            && glfwWindowShouldClose(win()) == 0
            && dx * dx + dy * dy > CIRCLE_SCALE*CIRCLE_SCALE
            );
}

int main(int argc, char *argv[]) {
    MyWin win;
    win.Init(600, 600, "AGL3 example", 0, 33);
    win.MainLoop();
    return 0;
}
