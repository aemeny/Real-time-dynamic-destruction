#include "BoxCollider.h"
#include "TraceRay.h"
#include "..\GameEngine\ModelRenderer.h"

namespace GameEngine
{
    BoxCollider::~BoxCollider() {}

    void BoxCollider::initialize()
    {
        //m_faces = m_entity.lock()->findComponent<GameEngine::ModelRenderer>().lock()->getModel().lock()->getFaces();
       
        m_transform = m_entity.lock()->findComponent<GameEngine::Transform>();
        m_colliderSize = glm::vec3(0);
        
        m_lineRendererDirty = true;
        m_renderOutline = false;
        core().lock()->m_traceRay->addObject(*this);
    }

    /*
    * onTick() checks through each box collider to see if they are intersecting and then responsed accordingly
    */
    void BoxCollider::onTick()
    {
        if(m_renderOutline)
            updateOutline();
    }

    void BoxCollider::onDisplay()
    {
        if(m_renderOutline)
            m_lineRenderer.lock()->renderLine(transform().lock()->getModelMatrix());
    }

    intersectionInfo BoxCollider::rayIntersect(Ray _ray)
    {
        intersectionInfo info;

        glm::vec3 localX = glm::normalize(glm::vec3(1.0, 0.0, 0.0));  // Local X axis
        glm::vec3 localY = glm::normalize(glm::vec3(0.0, 1.0, 0.0));  // Local Y axis
        glm::vec3 localZ = glm::normalize(glm::vec3(0.0, 0.0, 1.0));  // Local Z axis

        glm::mat3 boxRotation = glm::mat3(localX, localY, localZ);

        glm::vec3 localRayOrigin = (_ray.origin - m_transform.lock()->getPos()) * glm::inverse(boxRotation);
        glm::vec3 localRayDirection = _ray.direction * glm::inverse(boxRotation);

        float tMin = -std::numeric_limits<float>::infinity();
        float tMax = std::numeric_limits<float>::infinity();

        for (int i = 0; i < 3; ++i)
        {
            float t1 = (-(m_colliderSize[i] * 0.5f) - localRayOrigin[i]) / localRayDirection[i];
            float t2 = ((m_colliderSize[i] * 0.5f) - localRayOrigin[i]) / localRayDirection[i];

            tMin = glm::max(tMin, glm::min(t1, t2));
            tMax = glm::min(tMax, glm::max(t1, t2));
        }

        if (tMax >= tMin && tMax >= 0.0f)
        {
            info.hasIntersected = true;

            // Compute the intersection point along the ray
            glm::vec3 intersectionPoint = _ray.origin + tMin * _ray.direction;
            // Compute the distance from the ray's origin to the intersection point
            info.distanceToIntersection = glm::length(intersectionPoint - _ray.origin);
        }

        return info;
    }


    void BoxCollider::updateOutline()
    {
        if (m_lineRendererDirty)
        {
            m_lineRendererDirty = false;

            // Attach line renderer
            std::vector<std::shared_ptr<GameEngine::LineRenderer> > lineRenderer;
            core().lock()->find<GameEngine::LineRenderer>(lineRenderer);
            m_lineRenderer = lineRenderer[0];

            // Create new vbo for this box collider
            m_vbo = m_lineRenderer.lock()->addVbo();

            // Add positions
            glm::vec3 colliderPos = m_transform.lock()->getPos();
            glm::vec3 halfColliderSize = m_colliderSize * 0.5f;

            glm::vec3 pos1(colliderPos.x + halfColliderSize.x, colliderPos.y + halfColliderSize.y, colliderPos.z + halfColliderSize.z);
            glm::vec3 pos2(colliderPos.x + halfColliderSize.x, colliderPos.y + halfColliderSize.y, colliderPos.z - halfColliderSize.z);
            glm::vec3 pos3(colliderPos.x + halfColliderSize.x, colliderPos.y - halfColliderSize.y, colliderPos.z + halfColliderSize.z);
            glm::vec3 pos4(colliderPos.x - halfColliderSize.x, colliderPos.y + halfColliderSize.y, colliderPos.z + halfColliderSize.z);
            glm::vec3 pos5(colliderPos.x - halfColliderSize.x, colliderPos.y - halfColliderSize.y, colliderPos.z - halfColliderSize.z);
            glm::vec3 pos6(colliderPos.x - halfColliderSize.x, colliderPos.y - halfColliderSize.y, colliderPos.z + halfColliderSize.z);
            glm::vec3 pos7(colliderPos.x - halfColliderSize.x, colliderPos.y + halfColliderSize.y, colliderPos.z - halfColliderSize.z);
            glm::vec3 pos8(colliderPos.x + halfColliderSize.x, colliderPos.y - halfColliderSize.y, colliderPos.z - halfColliderSize.z);

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
