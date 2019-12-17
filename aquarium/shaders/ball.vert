#version 330 core

layout(location = 0) in vec3 position;
//layout(location = 1) in mat4 mvp;

//out vec3 normal;
out vec3 pos;

void main(){
    gl_Position.xyz = position/3;
    pos = gl_Position.xyz;
}