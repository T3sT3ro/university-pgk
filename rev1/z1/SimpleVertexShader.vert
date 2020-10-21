#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition_modelspace;
out vec3 position;
void main(){

    gl_Position.xyz = vertexPosition_modelspace;
    gl_Position.w = 1.0;
    position = gl_Position.xyz;
}

