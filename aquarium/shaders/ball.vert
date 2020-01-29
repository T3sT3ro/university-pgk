#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in mat4 modelMatrix;

layout(std140) uniform VPMatrices
{
    mat4 viewMatrix;
    mat4 projectionMatrix;
};

out vec3 normal;
out vec3 pos;

void main() {
    normal = normalize(position.xyz); // normal for ball is along radius
    gl_Position = projectionMatrix*viewMatrix*modelMatrix*vec4(position, 1);
    pos = gl_Position.xyz;
}