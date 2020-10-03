#version 330

layout(std140) uniform globals
{
    mat4 V;
    mat4 P;
    vec4 cameraPosWorld;
    vec4 lightPosWorld;
};

in vec3 fragPosWorld;
in vec3 normalCam; // normal in view space
in vec3 eyeDirCam; // eye in view space
in vec3 lightDirCam; // light pos in view space
in float DEPTH; //

out vec4 color;


vec3 ambientColor = vec3(.72,1,.96); // yellow-ish
vec3 objColor = vec3(.2,.4,.6); // feint-dark-blue-ish
vec3 lightColor = vec3(.8,.8,.2); // crimson-red
vec3 fadeout = vec3(.29,.4,.55); // color of fadeout for faraway objects
float lightPower = 50.0f;

void main() {

    float distance = length(lightPosWorld.xyz-fragPosWorld); // for light intensity

    vec3 n = normalize(normalCam); // normal of fragment
    vec3 l = normalize(lightDirCam); // from fragment to light
    float cosTheta = clamp(dot(n,l), 0, 1); // angle-lightDir angle
    vec3 E = normalize(eyeDirCam);
    vec3 R = reflect(-l, n);
    float cosAlpha = clamp(dot(E,R), 0, 1); // eye-reflection angle

    color =
    mix(
    vec4(objColor*(ambientColor * .4 + ((lightColor * lightPower/2*cosTheta + lightColor*lightPower * pow(cosAlpha,32)))/pow(distance, 2)), 1)
    , vec4(fadeout, .3), clamp(DEPTH*20, 0, 1))
    ;
}
