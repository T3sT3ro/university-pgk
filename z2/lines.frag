#version 330 core

vec3 colors[2]=vec3[](
    vec3(.7,.0,.0),
    vec3(.0,.0,.8)
);

// uniform int lastID;
out vec3 color;
uniform int lastFig;

void main(){
    color=mix(colors[gl_PrimitiveID%2],vec3(1),(gl_PrimitiveID==lastFig?.3:0));
}