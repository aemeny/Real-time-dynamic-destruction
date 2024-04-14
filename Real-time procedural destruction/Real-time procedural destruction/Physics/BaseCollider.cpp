#include "BaseCollider.h"
#include "RayTracer.h"

namespace GameEngine
{
    // Override intersection
    finalIntersection BaseCollider::rayIntersect(Ray _ray)
    {
        return finalIntersection();
    }
}

