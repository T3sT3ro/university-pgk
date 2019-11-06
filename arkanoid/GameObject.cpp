//
// Created by tooster on 05.11.2019.
//

#ifndef GAMEOBJECT
#define GAMEOBJECT

#include <initializer_list>
#include "common.hpp"
#include "Mesh.cpp"

using namespace glm;
using namespace std;

class GameObject {
public:
    struct AABB {
    public:
        vec2 pos, dim; // position and dimensions. left upper corner and width,height
        /// AABB intersection check. returns true if intersection occurs and false otherwise
        bool operator*(const AABB &other) {
            return pos.x < other.pos.x + other.dim.x &&
                   pos.x + dim.x > other.pos.x &&
                   pos.y < other.pos.y + other.dim.y &&
                   pos.y + dim.y > other.pos.y;
        }
    };

    AABB aabb{{0,0},{0,0}};

    struct Transform{
        vec2 position, scale;
        vec1 rotation;
    } transform;
    vec2 velocity{0,0};
    //todo vec2 angularVelocity;
    Mesh mesh;

    virtual void onCollide(GameObject &other){}
};

#endif