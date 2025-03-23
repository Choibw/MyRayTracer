#pragma once
#include "Ray.h"

// Scene 내 모든 객체(Plane, Sphere)가 상속
class Surface {
public:
    virtual ~Surface() {}

    // 교차 함수:
    //  - ray: 광선
    //  - tMin, tMax: 교차 검사 범위
    //  - out tHit: 교차된 t값 (가장 가까운)
    // 반환값: 교차하면 true, 아니면 false
    virtual bool intersect(const Ray& ray,
        float tMin, float tMax,
        float& tHit) const = 0;
};
