//
// Created by tooster on 05.11.2019.
//

#ifndef GAMEOBJECT
#define GAMEOBJECT

#include "common.hpp"
#include "Mesh.hpp"
#include "Geometry2D.hpp"
#include <algorithm>

using namespace std;

class GameObject {

    Transform2D transform; // pointer to transform so all transforms can be put inside one

    GameObject() = delete;
public:
    const GLuint instance;
    Mesh         *mesh; // mesh can be shared among objects
    vec2 velocity;
    BoxCollider aabb{{0, 0},
                     {0, 0}}; // broad collision detection

    GameObject(Mesh *mesh, GLuint instance = 0) : instance(instance), velocity(0) {
        recalcAABB();
    }


    void recalcAABB() {
        auto MVP = !transform;
        vec3 pmin = {FLT_MAX, FLT_MAX, FLT_MAX}, pmax = {FLT_MIN, FLT_MIN, FLT_MIN};
        for (const vec3 &point : mesh->points) {
            vec3 p = MVP * vec4(point, 1);
            pmin = {std::min(p.x, pmin.x), std::min(p.y, pmin.y), std::min(p.z, pmin.z)};
            pmax = {std::max(p.x, pmax.x), std::max(p.y, pmax.y), std::max(p.z, pmin.z)};
        }
        aabb = {{pmin}, {pmax-pmin}};
    }

    virtual void update(float dt) {}

    virtual void onCollide(GameObject &other){}

    virtual ~GameObject() {}
};

#endif
