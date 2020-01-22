//
// Created by tooster on 19.01.2020.
//

#ifndef PGK_GAMEOBJECT_HPP
#define PGK_GAMEOBJECT_HPP

#include "common.hpp"
#include "Shader.hpp"
#include "Renderer.hpp"

using namespace std;
using namespace glm;


struct Transform3D {
    vec3 translation = {0, 0, 0};
    vec3 scale       = {1, 1, 1};
    vec3 rotation    = {0, 0, 0};


    /// @return transformation 4x4 matrix for this transform
    mat4 toModelMatrix();
};




struct SphereCollider {
    vec3  position = {0, 0, 0};
    float radius   = 0;

    bool operator*(const SphereCollider &other);

    bool contains(const vec3 &point) const;
};




class GameObject {
protected:
    explicit GameObject(GLuint instance = 0);

public:
    const GLuint   instance;
    Transform3D    transform; // pointer to transform so all transforms can be put inside one
    SphereCollider collider;
    vec3           velocity; // normalised direction of motion
    float          speed; // scaling for velocity
    Renderer       *renderer{};

    virtual void update(float) = 0;

    virtual ~GameObject() = default;
};

#endif //PGK_GAMEOBJECT_HPP
