#version 330 core


// uniform int lastID;
out vec3 color;
in vec2 position;

uniform float dt;

#define PI 3.14159265

void main(){

    float f = 4.0;
    float timescale = .2;
    float val = .8- max(max(max(
        sin(f*(dt*timescale*1.2+distance(position, vec2(-.8,.2))))/7.0+.5,
        sin(f/2*(-dt*timescale+distance(position, vec2(.8,.2))))/7.0+.5),
        sin(f*(-dt*timescale+distance(position, vec2(.4,.3))))/7.0 + .5),
        sin(f*(-dt*timescale*0.5+distance(position, vec2(-.7,-.7))))/7.0 + .5);
    color=vec3(val);
}