#include "BaseCollider.h"
#include "TraceRay.h"

namespace GameEngine
{
    // Override intersection
    intersectionInfo BaseCollider::rayIntersect(Ray _ray)
    {
        return intersectionInfo();
    }

    void BaseCollider::updateOutline() {}
}

