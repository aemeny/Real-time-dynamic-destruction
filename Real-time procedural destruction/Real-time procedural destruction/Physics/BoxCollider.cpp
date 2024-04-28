#include "BoxCollider.h"
namespace GameEngine
{
    BoxCollider::~BoxCollider() {}

    void BoxCollider::initialize()
    {
        m_transform = m_entity.lock()->findComponent<GameEngine::Transform>();
    }

    /*
    * onTick() checks through each box collider to see if they are intersecting and then responsed accordingly
    */
    void BoxCollider::onTick()
    {
        /*
        isColliding = false;
        std::vector<std::shared_ptr<BoxCollider> > boxColliders;
        core().lock()->find<BoxCollider>(boxColliders);

        for (std::shared_ptr<BoxCollider> otherCollider : boxColliders)
        {
            if (otherCollider->m_entity.lock() != m_entity.lock()) // Dont want to collide with self
            {
                if (isCollidingCheck(otherCollider)) {
                    isColliding = true;
                    if (!staticObject)
                    {
                        getCollisionResponse(otherCollider);
                    }
                }
            }
        }*/
    }

    finalIntersection BoxCollider::rayIntersect(Ray _ray)
    {
        finalIntersection info;

        glm::mat4 modelMatrix = m_transform.lock()->getModelMatrix();

        glm::mat4 invModelMatrix = glm::inverse(modelMatrix);
        glm::vec4 rayOrigin4(_ray.origin, 1.0f);
        glm::vec4 rayDirection4(_ray.direction, 0.0f);
        glm::vec3 invRayOrigin = invModelMatrix * rayOrigin4;
        glm::vec3 invRayDirection = invModelMatrix * rayDirection4;

        // Ray-box intersection algorithm
        float tmin = -std::numeric_limits<float>::infinity();
        float tmax = std::numeric_limits<float>::infinity();

        for (int i = 0; i < 3; ++i) {
            float invRayDir = 1.0f / invRayDirection[i];
            float t1 = (-m_colliderSize[i] - invRayOrigin[i]) * invRayDir;
            float t2 = (m_colliderSize[i] - invRayOrigin[i]) * invRayDir;

            tmin = glm::max(tmin, glm::min(t1, t2));
            tmax = glm::min(tmax, glm::max(t1, t2));
        }

        if (tmax >= tmin)
            info.hasIntersected = true;

        return info;
    }
}
