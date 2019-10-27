#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <ctime>
#include <vector>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>

#undef DEBUG
#ifdef DEBUG
#define VERBOSE(X) \
    do {           \
        X;         \
    } while (0)
#else
#define VERBOSE(X) \
    {}
#endif

using namespace glm;

enum MODE { SEGMENT  = 2,
            TRIANGLE = 3 };

class GameManager {
   private:
    static constexpr float  radius  = .3f;
    static constexpr size_t CAP     = 256;  // initial endpoints capacity
    const float             epsilon = 1e-7;

   public:
    int   maxMistakes = 1;
    vec2 *endpoints;
    int   size     = 0;
    int   capacity = CAP;

    void (*resizeCallback)(vec2 *data);

    int  player      = 0;
    int  mistakes[2] = {0, 0};
    MODE mode        = SEGMENT;
    bool gameEnded   = true;
#define FIGSIZE (mode == SEGMENT ? 2 : 3)

    struct AABB {
        vec2 v1, v2;
    };

    GameManager() {
        srand(time(NULL));
        endpoints = new vec2[CAP];
    }

    /// Changes turn and returns id of player that acquired turn.
    inline int  nextTurn() { return (player ^= 1); }
    inline void restart() {
        player      = 0;
        mistakes[0] = mistakes[1] = 0;
        size                      = 0;
        gameEnded                 = false;
        if (resizeCallback != nullptr) resizeCallback(endpoints);
    }
    inline int    getFigureIdx(int i) { return i * FIGSIZE; }
    inline int    getLastFigureIdx() { return size - FIGSIZE; }  // return begining idx of last figure or < 0 if empty
    inline int    getFiguresCount() { return size / FIGSIZE; }
    inline size_t getFigureSize() { return FIGSIZE; }

    inline bool AABBoverlap(AABB &&aabb1, AABB &&aabb2) {
        return aabb1.v1.x <= aabb2.v2.x && aabb1.v2.x >= aabb2.v1.x &&
               aabb1.v1.y <= aabb2.v2.y && aabb1.v2.y >= aabb2.v1.y;
    }
    AABB calcAABB(vec2 *points) {
        float xMin = FLT_MAX, xMax = FLT_MIN, yMin = FLT_MAX, yMax = FLT_MIN;
        for (size_t i = 0; i < FIGSIZE; i++) {
            xMin = std::min(points[i].x, xMin);
            yMin = std::min(points[i].y, yMin);
            xMax = std::max(points[i].x, xMax);
            yMax = std::max(points[i].y, yMax);
        }
        return AABB{{xMin, yMin}, {xMax, yMax}};
    }
    inline int curl(vec2 a, vec2 b, vec2 c) {  // calculates curl of 3 consecutive endpoints, 0 is collinear
        float  d = (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
        if (d > -epsilon && d < epsilon)
            return 0;
        else
            return d < 0 ? -1 : 1;
    }
    inline bool insideAABB(AABB &&aabb, vec2 p) {
        return min(aabb.v1.x, aabb.v2.x) <= p.x && p.x <= max(aabb.v1.x, aabb.v2.x) &&
               max(aabb.v1.y, aabb.v2.y) <= p.y && p.y <= max(aabb.v1.y, aabb.v2.y);
    }
    inline bool lineIntersect(vec2 &v1, vec2 &v2, vec2 &w1, vec2 &w2) {
        int c1 = curl(v1, v2, w1), c2 = curl(v1, v2, w2);
        int c3 = curl(w1, w2, v1), c4 = curl(w1, w2, v2);
        if (c1 != c2 && c3 != c4 ||
            c1 == 0 && insideAABB({v1, v2}, w1) ||
            c2 == 0 && insideAABB({v1, v2}, w2) ||
            c3 == 0 && insideAABB({w1, w2}, v1) ||
            c2 == 0 && insideAABB({w1, w2}, v2))
            return true;
        else
            return false;
    }

    
    void putFigure(vec2 p1) {
        if (gameEnded) return;
        float t       = fmod(rand(), 2 * pi<float>());
        bool  resized = false;
        if (size + FIGSIZE > capacity) {
            endpoints         = new vec2[2 * capacity];
            auto oldendpoints = endpoints;
            std::copy(oldendpoints, oldendpoints + capacity, endpoints);
            delete[] oldendpoints;
            resized = true;
            capacity *= 2;
        }
        endpoints[size++] = p1;  // put first point
        VERBOSE(fprintf(stderr, "[P%d] put (%.3f, %.3f)", player, endpoints[size - 1].x, endpoints[size - 1].y));
        endpoints[size++] = vec2(p1.x + radius * cos(t), p1.y + radius * sin(t));  // random second point
        VERBOSE(fprintf(stderr, " (%.3f, %.3f)", endpoints[size - 1].x, endpoints[size - 1].y));
        if (mode == TRIANGLE) {  // 3rd point rotated 90 degrees from last point
            endpoints[size++] = vec2(p1.x + radius * cos(t + half_pi<float>()), p1.y + radius * sin(t + half_pi<float>()));
            VERBOSE(fprintf(stderr, " (%.3f, %.3f) ", endpoints[size - 1].x, endpoints[size - 1].y));
        }
        fprintf(stderr, "[vertices:%d, figures:%d]\n", size, getFiguresCount());
        if (resized)
            resizeCallback(endpoints);

        int  k    = getLastFigureIdx();
        bool miss = false;
        for (int i = 0, lim = getFiguresCount(), j = 0; i < lim - 1; ++i, j = getFigureIdx(i))  // check all intersections
            if (AABBoverlap(calcAABB(&endpoints[getLastFigureIdx()]), calcAABB(&endpoints[getFigureIdx(i)]))) {
                if (mode == SEGMENT)
                    miss |= lineIntersect(endpoints[j], endpoints[j + 1], endpoints[k], endpoints[k + 1]);
                else {
                    miss |= lineIntersect(endpoints[j], endpoints[j + 1], endpoints[k], endpoints[k + 1]) ||
                            lineIntersect(endpoints[j + 2], endpoints[j + 1], endpoints[k], endpoints[k + 1]) ||
                            lineIntersect(endpoints[j], endpoints[j + 2], endpoints[k], endpoints[k + 1]) ||
                            lineIntersect(endpoints[j], endpoints[j + 1], endpoints[k + 2], endpoints[k + 1]) ||
                            lineIntersect(endpoints[j + 2], endpoints[j + 1], endpoints[k + 2], endpoints[k + 1]) ||
                            lineIntersect(endpoints[j], endpoints[j + 2], endpoints[k + 2], endpoints[k + 1]) ||
                            lineIntersect(endpoints[j], endpoints[j + 1], endpoints[k + 2], endpoints[k + 1]) ||
                            lineIntersect(endpoints[j + 2], endpoints[j + 1], endpoints[k + 2], endpoints[k + 1]) ||
                            lineIntersect(endpoints[j], endpoints[j + 2], endpoints[k + 2], endpoints[k + 1]);  // lul
                }
            }
        if (miss) mistakes[player]++;
        VERBOSE(fprintf(stderr, "!! P%d %s\n", player, (miss ? "MISS" : "PASS")));

        if (mistakes[player] >= maxMistakes) {
            fprintf(stderr, "P%d LOSES", player);
            gameEnded = true;
        }

        nextTurn();
    }

#undef FIGSIZE
};
