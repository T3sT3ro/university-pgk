#version 330
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 uv;
layout(location = 2) in mat4 modelMatrix;

layout(std140) uniform globals
{
    mat4 V;
    mat4 P;
    vec4 cameraPosWorld;
    vec4 lightPosWorld;
};

// all calulations in view space
void main() {
    gl_Position = P*V*modelMatrix*vec4(position, 1);
}