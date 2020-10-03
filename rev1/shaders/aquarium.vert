#version 330
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 uv;
layout(location = 3) in vec3 normal;
layout(location = 4) in mat4 modelMatrix;

layout(std140) uniform globals
{
    mat4 V;
    mat4 P;
    vec4 cameraPosWorld;
    vec4 lightPosWorld;
};

uniform float FAR;

out vec3 fragPosWorld;
out vec3 normalCam; // normal in view space
out vec3 eyeDirCam; // eye in view space
out vec3 lightDirCam; // light pos in view space
out float DEPTH; //

// all calulations in view space
void main() {
    gl_Position = P*V*modelMatrix*vec4(position, 1);

    fragPosWorld = vec3(modelMatrix*vec4(position, 1.0)); // fragment position in world
    normalCam = (V*modelMatrix*(vec4(normal, 0))).xyz; // flip normals for aquarium
    eyeDirCam = -(V*modelMatrix*vec4(position, 1)).xyz; // vector to the camera from vertex
    lightDirCam = eyeDirCam + (V*lightPosWorld).xyz;
    DEPTH = gl_Position.z / FAR;
}