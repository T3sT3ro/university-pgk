#version 330

#extension GL_ARB_explicit_uniform_location : require
#extension GL_ARB_shading_language_420pack : require

layout(location = 0) uniform float length;
layout(location = 0) in vec2 center;
layout(location = 1) in float rotation;

out vec4 f_color;

void main(void) {
    mat2 rotMat = transpose(mat2(
    cos(rotation), -sin(rotation),
    sin(rotation), cos(rotation)
    ));

    float vertexMod2 = mod(gl_VertexID, 2.0);
    vec2 endpoint = vec2((vertexMod2*2.0-1.0)*length, 0);// x of endpoint is -length/+length for even/odd
    endpoint = rotMat * endpoint + vec2(center);
    gl_Position = vec4(endpoint, 0, 1);
    if(gl_InstanceID == 0){
        f_color = vec4(vertexMod2, 0, 1-vertexMod2, 1);
    } else {
        f_color = vec4(vertexMod2, 1-vertexMod2, 0, 1);
    }
}
