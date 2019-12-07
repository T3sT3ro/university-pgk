//
// Created by tooster on 05.11.2019.
//

#ifndef GAMEOBJECT
#define GAMEOBJECT

#include "common.hpp"
#include "Shader.cpp"

using namespace std;

struct Transform3D {
    vec3 translation = {0, 0, 0};
    vec3 scale       = {1, 1, 1};
    vec3 rotation    = {0, 0, 0};


    /// @return transformation 4x4 matrix for this transform
    mat4 operator!() {
        glm::mat4 mm = mat4(1);
        mm = glm::scale(mm, scale);
        mm = glm::rotate(mm, glm::radians(rotation.z), {0, 0, 1});
        mm = glm::rotate(mm, glm::radians(rotation.y), {0, 1, 0});
        mm = glm::rotate(mm, glm::radians(rotation.x), {1, 0, 0});
        mm = glm::translate(mm, translation);
        return mm;
    }
};

struct SphereCollider {
    vec3  position = {0, 0, 0};
    float radius   = 0;

    bool operator*(const SphereCollider &other) {
        return radius > 0 && distance(position, other.position) <= radius + other.radius;
    }

    bool contains(const vec3 &point) const { return radius > 0 && glm::distance(point, position) <= radius; }
};

// todo: ShaderBoundProperty

class GameObject {

public:
    const GLuint   instance;
    Transform3D    transform; // pointer to transform so all transforms can be put inside one
    SphereCollider collider;
    vec3           velocity; // normalised direction of motion
    float          speed; // scaling for velocity
    Renderer       *renderer;

    GameObject(GLuint instance = 0) : instance(instance), velocity(0) {}

    virtual void update(float dt) {}

    virtual ~GameObject() {}
};

#endif
