#version 330 core

in vec3 pos;
in vec3 normal;

out vec3 color;
//uniform vec3 lightPosition;

layout(std140) uniform Lights
{
    vec3 lightPos; // rgb, w is intensity
};

void main() {
    float ambientStr = 0.1;
    vec3 ambientColor = vec3(0.95, 0.98, 0.70);
    vec3 objColor = vec3(.84,.91,.41);

//    color = (ambientStr * ambientColor + max(dot(normal, vec3(1, 1, -1)), 0.0)) * objColor;
    color = vec3(1,0,0);
}
