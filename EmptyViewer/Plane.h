#pragma once
#include "Surface.h"
#include <cmath>

// Plane: n·x + d = 0 형태
// 과제: y = -2  => n=(0,1,0), d=2
class Plane : public Surface {
public:
    Plane(const glm::vec3& normal, float dVal)
        : n(glm::normalize(normal)), d(dVal) {}

    bool intersect(const Ray& ray,
        float tMin, float tMax,
        float& tHit) const override
    {
        float denom = glm::dot(n, ray.direction);
        // 광선과 평면이 거의 평행
        if (fabs(denom) < 1e-6f) {
            return false;
        }
        // t = -(n·origin + d) / (n·dir)
        float t = -(glm::dot(n, ray.origin) + d) / denom;
        if (t < tMin || t > tMax) {
            return false;
        }
        tHit = t;
        return true;
    }

private:
    glm::vec3 n;  // 법선
    float d;      // n·x + d = 0
};

