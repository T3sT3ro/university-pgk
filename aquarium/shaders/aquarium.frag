#version 330

layout(std140) uniform Lights
{
    vec3 lightPos; // rgb, w is intensity
};

in float DEPTH;
in vec3 normal;
in vec3 fragPos;
in vec2 TexCoord;

out vec3 color;

uniform sampler2D ourTexture;

/// noise
vec2 u_resolution = vec2(800, 800);
float random (in vec2 st) {
    return fract(sin(dot(st.xy,
    vec2(12.9898,78.233)))
    * 43758.5453123);
}

// Value noise by Inigo Quilez - iq/2013
// https://www.shadertoy.com/view/lsf3WH
float noise(vec2 st) {
    vec2 i = floor(st);
    vec2 f = fract(st);
    vec2 u = f*f*(3.0-2.0*f);
    return mix( mix( random( i + vec2(0.0,0.0) ),
    random( i + vec2(1.0,0.0) ), u.x),
    mix( random( i + vec2(0.0,1.0) ),
    random( i + vec2(1.0,1.0) ), u.x), u.y);
}

mat2 rotate2d(float angle){
    return mat2(cos(angle),-sin(angle),
    sin(angle),cos(angle));
}

float lines(in vec2 pos, float b){
    float scale = 10.0;
    pos *= scale;
    return smoothstep(0.0,
    .5+b*.5,
    abs((sin(pos.x*3.1415)+b*2.0))*.5);
}
///

void main() {
    vec3 lightColor = vec3(.45,.58,.84); // player's color
    vec3 objColor = vec3(.13,.54,.94);
    vec3 playerColor = vec3(.84,.91,.41); // aquarium color

    vec3 ambient = 0.1 * lightColor;

    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(lightPos-fragPos);

    vec3 diffuse = max(dot(norm, lightDir), 0.0)*lightColor;

    vec3 result = texture(ourTexture, TexCoord).xyz; //(ambient+diffuse)*objColor;
//    color = result; //mix(result, vec3(.4,.16,.25), DEPTH*10);
// -----
    vec2 st = gl_FragCoord.xy/u_resolution.xy;
    st.y *= u_resolution.y/u_resolution.x;

    vec2 pos = st.yx*vec2(10.,3.);

    float pattern = pos.x;

    // Add noise
    pos = rotate2d( noise(pos) ) * pos;

    // Draw lines
    pattern = lines(pos,.5);

    color = vec4(vec3(pattern),1.0).xyz;
// ----
}
