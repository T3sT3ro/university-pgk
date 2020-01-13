#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in mat4 modelMatrix;

layout(std140) uniform Lights
{
    vec4 light; // xyz is position, w is intensity
};

layout(std140) uniform VPMatrices
{
    mat4 viewMatrix;
    mat4 projectionMatrix;
};

uniform bool flipNormals;

out vec3 normal;
out vec3 pos;

void main() {
    mat4 ident = mat4(1);
    if(flipNormals) normal = normalize(-position);
    else normal = normalize(position);

    gl_Position = viewMatrix*vec4(position, 1);
    pos = gl_Position.xyz;
}