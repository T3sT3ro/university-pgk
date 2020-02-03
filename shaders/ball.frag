#version 330

layout(std140) uniform globals
{
    mat4 V;
    mat4 P;
    vec4 cameraPosWorld;
    vec4 lightPosWorld;
};

out vec4 color;

void main() {

    color = vec4(1,1,.9,1);
}
