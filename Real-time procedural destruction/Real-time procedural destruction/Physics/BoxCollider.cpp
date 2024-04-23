#include "BoxCollider.h"
namespace GameEngine
{
    BoxCollider::~BoxCollider() {}

    void BoxCollider::initialize()
    {
        m_transform = m_entity.lock()->findComponent<GameEngine::Transform>();
        m_dirty = true;
        m_colliderSize = glm::vec3(0);
    }

    /*
    * onTick() checks through each box collider to see if they are intersecting and then responsed accordingly
    */
    void BoxCollider::onTick()
    {
        drawBoxOutline(true);

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
        
        //glm::mat4 invModelMatrix = glm::inverse(modelMatrix);
        glm::mat4 invModelMatrix = glm::mat4(1);
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


    void BoxCollider::drawBoxOutline(bool _draw)
    {
        if (_draw)
        {
            if (m_dirty)
            {
                m_dirty = false;
                
                // Attach line renderer
                std::vector<std::shared_ptr<GameEngine::LineRenderer> > lineRenderer;
                core().lock()->find<GameEngine::LineRenderer>(lineRenderer);
                m_lineRenderer = lineRenderer[0];

                // Create new vbo for this box collider
                m_vbo = m_lineRenderer.lock()->addVbo();
                
                // Add positions
                glm::vec3 colliderPos = m_transform.lock()->getPos();
                glm::vec3 halfColliderSize = m_colliderSize;
                
                glm::vec3 pos1 (colliderPos.x + halfColliderSize.x, colliderPos.y + halfColliderSize.y, colliderPos.z + halfColliderSize.z);
                glm::vec3 pos2 (colliderPos.x + halfColliderSize.x, colliderPos.y + halfColliderSize.y, colliderPos.z - halfColliderSize.z);
                glm::vec3 pos3 (colliderPos.x + halfColliderSize.x, colliderPos.y - halfColliderSize.y, colliderPos.z + halfColliderSize.z);
                glm::vec3 pos4 (colliderPos.x - halfColliderSize.x, colliderPos.y + halfColliderSize.y, colliderPos.z + halfColliderSize.z);
                glm::vec3 pos5 (colliderPos.x - halfColliderSize.x, colliderPos.y - halfColliderSize.y, colliderPos.z - halfColliderSize.z);
                glm::vec3 pos6 (colliderPos.x - halfColliderSize.x, colliderPos.y - halfColliderSize.y, colliderPos.z + halfColliderSize.z);
                glm::vec3 pos7 (colliderPos.x - halfColliderSize.x, colliderPos.y + halfColliderSize.y, colliderPos.z - halfColliderSize.z);
                glm::vec3 pos8 (colliderPos.x + halfColliderSize.x, colliderPos.y - halfColliderSize.y, colliderPos.z - halfColliderSize.z);
                
                m_lineRenderer.lock()->addLine(m_vbo, pos1, pos2);
                m_lineRenderer.lock()->addLine(m_vbo, pos1, pos3);
                m_lineRenderer.lock()->addLine(m_vbo, pos1, pos4);
                m_lineRenderer.lock()->addLine(m_vbo, pos5, pos6);
                m_lineRenderer.lock()->addLine(m_vbo, pos5, pos7);
                m_lineRenderer.lock()->addLine(m_vbo, pos5, pos8);
                m_lineRenderer.lock()->addLine(m_vbo, pos6, pos3);
                m_lineRenderer.lock()->addLine(m_vbo, pos8, pos3);
                m_lineRenderer.lock()->addLine(m_vbo, pos8, pos2);
                m_lineRenderer.lock()->addLine(m_vbo, pos7, pos2);
                m_lineRenderer.lock()->addLine(m_vbo, pos7, pos4);
                m_lineRenderer.lock()->addLine(m_vbo, pos6, pos4);
            }
            else
            {
                // IF CHANGED POSITION
                // CLEAR THE POSITIONS
                // ADD NEW POSITIONS
            }
        }
    }
}
