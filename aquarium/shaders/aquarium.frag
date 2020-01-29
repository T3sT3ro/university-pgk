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

void main() {
    vec3 lightColor = vec3(.45,.58,.84); // player's color
    vec3 objColor = vec3(.13,.54,.94);
    vec3 playerColor = vec3(.84,.91,.41); // aquarium color

    vec3 ambient = 0.1 * lightColor;

    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(lightPos-fragPos);

    vec3 diffuse = max(dot(norm, lightDir), 0.0)*lightColor;

    vec3 result = texture(ourTexture, TexCoord).xyz; //(ambient+diffuse)*objColor;
    color = result; //mix(result, vec3(.4,.16,.25), DEPTH*10);
//    vec3(.2,.7,.9), vec3(.4,.5,.6)
}
