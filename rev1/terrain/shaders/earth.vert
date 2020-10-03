#version 330
layout(location = 0) in int height;
layout(location = 2) in mat4 modelMatrix;

layout(std140) uniform globals
{
    mat4 V;
    mat4 P;
    vec4 cameraPosWorld;
    vec4 lightPosWorld;
};

out float ht;

void main() {
    gl_Position = P*V*modelMatrix*vec4(position, 1);
    ht = position.z;
}