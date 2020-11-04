//
// Created by tooster on 04.11.2020.
// Source from https://www.geeksforgeeks.org/check-if-two-given-line-segments-intersect/
//

#ifndef PGK_INTERSECTION_HPP
#define PGK_INTERSECTION_HPP

#include <glm/glm.hpp>
#include <algorithm>

// Given three collinear points p, q, r, the function checks if
// point q lies on line segment 'pr'
bool onSegment(const glm::vec2 &p, const glm::vec2 &q, const glm::vec2 &r) {
    if (std::min(p.x, r.x) <= q.x && q.x <= std::max(p.x, r.x) &&
        std::min(p.y, r.y) <= q.y && q.y <= std::max(p.y, r.y))
        return true;

    return false;
}

// To find orientation of ordered triplet (p, q, r).
// The function returns following values
// 0 --> p, q and r are colinear
// 1 --> Clockwise
// 2 --> Counterclockwise
int orientation(const glm::vec2 &p, const glm::vec2 &q, const glm::vec2 &r) {
    float cross = (q.y - p.y) * (r.x - q.x) - (q.x - p.x) * (r.y - q.y);

    if (std::abs(cross) < 1e-4) return 0;

    return (cross > 0) ? 1 : 2;
}

/// returns true if segment (a1, a2) and (b1, b2) intersect (+- epsilon)
bool intersectSegmentSegment(const glm::vec2 &a1, const glm::vec2 &a2, const glm::vec2 &b1, const glm::vec2 &b2) {
    int o1 = orientation(a1, a2, b1);
    int o2 = orientation(a1, a2, b2);
    int o3 = orientation(b1, b2, a1);
    int o4 = orientation(b1, b2, a2);

    // General case
    if (o1 != o2 && o3 != o4)
        return true;

    // Special Cases
    if (o1 == 0 && onSegment(a1, b1, a2)) return true;
    if (o2 == 0 && onSegment(a1, b2, a2)) return true;
    if (o3 == 0 && onSegment(b1, a1, b2)) return true;
    if (o4 == 0 && onSegment(b1, a2, b2)) return true;

    return false;
}

#endif //PGK_INTERSECTION_HPP
