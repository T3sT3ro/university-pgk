#version 330
#extension GL_ARB_explicit_uniform_location : require

in vec4 f_color;
out vec4 color;

void main(void) {
    color = f_color;
}