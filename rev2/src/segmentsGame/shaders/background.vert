#version 330

#extension GL_ARB_explicit_uniform_location : require
#extension GL_ARB_shading_language_420pack : require

layout(location = 0) uniform float time;

out vec2 f_pos;

void main(void) {
    const vec2 vertices[4] = vec2[4](
    vec2(-0.9, -0.9),
    vec2(0.9, -0.9),
    vec2(0.9, 0.9),
    vec2(-0.9, 0.9)
    );

    gl_Position = vec4(vertices[gl_VertexID], 0.5, 1.0);

    f_pos = vertices[gl_VertexID] + vec2(cos(time/2), sin(time/5));
}
