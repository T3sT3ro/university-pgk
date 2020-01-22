#version 330

out vec3 color;

void main() {
    color = mix(vec3(0,1,0), vec3(1,0,0), gl_FragCoord.z);
//    vec3(.2,.7,.9), vec3(.4,.5,.6)
}
