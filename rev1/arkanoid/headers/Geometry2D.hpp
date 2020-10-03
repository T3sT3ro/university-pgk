//
// Created by tooster on 08.11.2019.
//

#ifndef ARKANOID_GEOMETRY2D_HPP
#define ARKANOID_GEOMETRY2D_HPP

#include "common.hpp"
#include <glm/gtx/closest_point.hpp>

using namespace glm;

constexpr const float EPSILON = 10e-7;

struct Transform2D {
    vec2  translation = {0, 0};
    vec2  scale       = {1, 1};
    float rotation    = 0;


    /// @return transformation 4x4 matrix for this transform
    mat4 operator!() {
        glm::mat4 mm = mat4(1);
        mm = glm::scale(mm, vec3(scale, 1));
        mm = glm::rotate(mm, glm::radians(rotation), {0, 0, 1});
        mm = glm::translate(mm, vec3(translation, 0));
        return mm;
    }
};

struct BoxCollider;
struct SphereCollider;
struct TrisCollider;

struct BoxCollider {
    vec2 position, size; // position and dimensions. left upper corner and width,height

    bool operator*(const BoxCollider &other);
    bool operator*(const SphereCollider &other);

    bool contains(const vec2 &point) const{
        return size.length() != 0 &&
               point.x >= position.x && point.x <= position.x + size.x &&
               point.y >= position.y && point.y <= position.y + size.y;
    }

    BoxCollider expand(const vec2 &xy) { return BoxCollider{position - xy, size + xy + xy}; }
};

struct SphereCollider {
    vec2  position;
    float radius;

    bool operator*(const SphereCollider &other);
    bool operator*(const TrisCollider &other);

    bool contains(const vec2 &point) const {
        return glm::distance(point, position) <= radius;
    }
};

inline int curl(vec2 a, vec2 b, vec2 c) {  // calculates curl of 3 consecutive endpoints, 0 is collinear, 1 for CCW
    float d = (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
    if (-EPSILON < d && d < EPSILON)
        return 0;
    else
        return d < 0 ? -1 : 1;
}

struct TrisCollider {
    const vec2 points[3];

    bool operator*(const TrisCollider &other);

    bool contains(const vec2 &point) const {
        int c1 = curl(points[0], points[1], point),
            c2 = curl(points[1], points[2], point),
            c3 = curl(points[2], points[0], point);
        return (c1 == c2 || c1 == 0 || c2 == 0) && (c1 == c3 || c1 == 0 || c3 == 0) && (c2 == c3 || c2 == 0 || c3 == 0);
    }
};

inline bool lineIntersect(const vec2 &v1, const vec2 &v2, const vec2 &w1, const vec2 &w2) {
    int c1 = curl(v1, v2, w1), c2 = curl(v1, v2, w2);
    int c3 = curl(w1, w2, v1), c4 = curl(w1, w2, v2);
    return c1 != c2 && c3 != c4 ||
           c1 == 0 && BoxCollider{v1, v2}.contains(w1) ||
           c2 == 0 && BoxCollider{v1, v2}.contains(w2) ||
           c3 == 0 && BoxCollider{w1, w2}.contains(v1) ||
           c2 == 0 && BoxCollider{w1, w2}.contains(v2);
}

/// AABB intersection check. returns true if intersection occurs and false otherwise
bool BoxCollider::operator*(const BoxCollider &other) {
    return position.x < other.position.x + other.size.x &&
           position.x + size.x > other.position.x &&
           position.y < other.position.y + other.size.y &&
           position.y + size.y > other.position.y;
}

bool BoxCollider::operator*(const SphereCollider &other) {
    return expand(vec2(other.radius, 0)).contains(other.position) ||
           expand(vec2(0, other.radius)).contains(other.position) ||
           std::min(
                   std::min(distance(other.position, position),
                            distance(other.position, position + vec2(size.x, 0))),
                   std::min(distance(other.position, position + size),
                            distance(other.position, position + vec2(0, size.y)))) <= other.radius;
}

bool SphereCollider::operator*(const SphereCollider &other) {
    return distance(position, other.position) <= radius + other.radius;
}

bool SphereCollider::operator*(const TrisCollider &other) {
    return other.contains(position) ||
           distance(position, glm::closestPointOnLine(position, other.points[0], other.points[1]))
           <= radius ||
           distance(position, glm::closestPointOnLine(position, other.points[1], other.points[2]))
           <= radius ||
           distance(position, glm::closestPointOnLine(position, other.points[2], other.points[0]))
           <= radius;
}

bool TrisCollider::operator*(const TrisCollider &other) {
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            if (lineIntersect(points[i], points[(i + 1) % 3], other.points[j], other.points[(j + 1) % 3]))
                return true;
    return false;
}

#endif //ARKANOID_GEOMETRY2D_HPP
