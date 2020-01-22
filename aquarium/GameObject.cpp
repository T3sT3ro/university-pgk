//
// Created by tooster on 05.11.2019.
//

#include "GameObject.hpp"

mat4 Transform3D::toModelMatrix() {
    auto mm = glm::scale(mat4(1.0f), scale);
    mm = glm::rotate(mm, glm::radians(rotation.z), {0, 0, 1});
    mm = glm::rotate(mm, glm::radians(rotation.y), {0, 1, 0});
    mm = glm::rotate(mm, glm::radians(rotation.x), {1, 0, 0});
    mm = glm::translate(mm, translation);
    return mm;
}




bool SphereCollider::operator*(const SphereCollider &other) {
        return radius > 0 && distance(position, other.position) <= radius + other.radius;
    }

bool SphereCollider::contains(const vec3 &point) const { return radius > 0 && glm::distance(point, position) <= radius; }




GameObject::GameObject(Renderer *renderer, GLuint instance) : instance(instance), renderer(renderer), velocity(0) {}
