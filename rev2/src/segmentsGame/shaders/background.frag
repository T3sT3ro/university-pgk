#version 330
#extension GL_ARB_explicit_uniform_location : require

in vec2 f_pos;

out vec4 color;

void main(void) {

    float chess = floor(f_pos.x*10) + floor(f_pos.y*10);
    chess = mod(chess*0.5, 1.0)*2;
    color = vec4(chess, chess, chess, 1) * 0.2;
}