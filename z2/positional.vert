#version 330 core

layout(location = 0) in vec2 pos;
out vec2 position;

void main(){
    gl_Position.xy  = pos;
    position = gl_Position.xy;
}