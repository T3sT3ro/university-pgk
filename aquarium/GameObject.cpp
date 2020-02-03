//
// Created by tooster on 05.11.2019.
//

#include "GameObject.hpp"

mat4 Transform3D::toModelMatrix() {
    mat4 mm(1);
    mm = glm::translate(mm, translation);
    mm = glm::rotate(mm, glm::radians(rotation.z), {0, 0, 1});
    mm = glm::rotate(mm, glm::radians(rotation.y), {0, 1, 0});
    mm = glm::rotate(mm, glm::radians(rotation.x), {1, 0, 0});
    mm = glm::scale(mm, scale);
    return mm;
}




bool SphereCollider::operator*(const SphereCollider &other) {
        return radius > 0 && distance(object->transform.translation, other.object->transform.translation) <= radius*object->transform.scale.x + other.radius*other.object->transform.scale.x;
    }

bool SphereCollider::contains(const vec3 &point) const { return radius > 0 && glm::distance(point, object->transform.translation) <= radius*object->transform.scale.x; }

SphereCollider::SphereCollider(GameObject *object) : object(object) {}


GameObject::GameObject(Renderer *renderer, GLuint instance) : instance(instance), renderer(renderer), velocity(0) {}
