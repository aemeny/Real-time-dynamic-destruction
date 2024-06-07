#include "MeshCollider.h"
#include <glm/gtx/intersect.hpp>
#include "..\GameEngine\ModelLoader.h"
#include "TraceRay.h"

namespace GameEngine
{
    MeshCollider::~MeshCollider() {}

    void MeshCollider::initialize()
    {
        m_faces = m_entity.lock()->findComponent<GameEngine::ModelLoader>().lock()->getModel().lock()->getFaces();
        m_lineRendererDirty = true;
        m_renderOutline = false;
        core().lock()->m_traceRay->addObject(*this);
    }

    /*
    * onTick() runs every frame
    */
    void MeshCollider::onTick()
    {
        drawOutline(m_renderOutline);
    }

    intersectionInfo MeshCollider::rayIntersect(Ray _ray)
    {
        intersectionInfo info;

        float nearest = -1.0f;
        glm::vec2 baryPosition = glm::vec2(0);
        float distance;
        
        for (size_t i = 0; i < m_faces->size(); i++)
        {
            if (glm::intersectRayTriangle(_ray.origin, _ray.direction,
                m_faces->at(i).pa, m_faces->at(i).pb, m_faces->at(i).pc, baryPosition, distance))
            {
                if (distance > 0.0f)
                {
                    if (nearest == -1 || distance < nearest)
                    {
                        nearest = distance;
                        info.hasIntersected = true;
                        info.intersectionPos = glm::vec3(baryPosition.x, baryPosition.y, 0);
                        info.distanceToIntersection = distance;
                        info.collidedFace = &m_faces->at(i);
                    }
                }
            }
        }

        if (info.hasIntersected)
            info.intersectedModel = m_entity.lock()->findComponent<GameEngine::ModelLoader>().lock()->getModel();
        
        return info;
    }

    void MeshCollider::drawOutline(bool _draw)
    {
        if (_draw)
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

                for (int i = 0; i < m_faces->size(); i++)
                {
                    glm::vec3 pos1(m_faces->at(i).pa.x, m_faces->at(i).pa.y, m_faces->at(i).pa.z);
                    glm::vec3 pos2(m_faces->at(i).pb.x, m_faces->at(i).pb.y, m_faces->at(i).pb.z);
                    glm::vec3 pos3(m_faces->at(i).pc.x, m_faces->at(i).pc.y, m_faces->at(i).pc.z);

                    m_lineRenderer.lock()->addLine(m_vbo, pos1, pos2);
                    m_lineRenderer.lock()->addLine(m_vbo, pos2, pos3);
                    m_lineRenderer.lock()->addLine(m_vbo, pos3, pos1);
                }
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
