#version 330
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 uv;
layout(location = 2) in mat4 modelMatrix;

layout(std140) uniform VPMatrices
{
    mat4 viewMatrix;
    mat4 projectionMatrix;
};

uniform float FAR;

out float DEPTH;
out vec3 normal;
out vec3 fragPos;
out vec2 TexCoord;

void main() {
    normal = -normalize(position); // flip normals for aquarium
    fragPos = vec3(modelMatrix*vec4(position, 1.0)); // interpolated fragment positing in world
    gl_Position = projectionMatrix*viewMatrix*modelMatrix*vec4(position, 1);
    DEPTH = gl_Position.z / FAR;
    TexCoord = uv;
}