#version 330

out vec3 color;

in float DEPTH;
void main() {
    color = mix(vec3(.2,.7,.9), vec3(.3,.2,.6), DEPTH*10);
//    vec3(.2,.7,.9), vec3(.4,.5,.6)
}
