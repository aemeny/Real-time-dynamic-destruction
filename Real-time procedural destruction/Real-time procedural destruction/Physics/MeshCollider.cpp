#include "MeshCollider.h"
#include <glm/gtx/intersect.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "TraceRay.h"

namespace GameEngine
{
    MeshCollider::~MeshCollider() {}

    void MeshCollider::initialize()
    {
        m_modelRenderer = m_entity.lock()->findComponent<GameEngine::ModelRenderer>();
        m_lineRendererDirty = true;
        m_renderOutline = false;
        core().lock()->m_traceRay->addObject(*this);
    }

    /*
    * onTick() runs every frame
    */
    void MeshCollider::onTick()
    {
        if(m_renderOutline)
            updateOutline();
    }

    void MeshCollider::onDisplay()
    {
        if (m_renderOutline)
            m_lineRenderer.lock()->renderLine(transform().lock()->getModelMatrix());
    }

    intersectionInfo MeshCollider::rayIntersect(Ray _ray)
    {
        intersectionInfo info;

        float nearest = -1.0f;
        glm::vec2 baryPosition = glm::vec2(0);
        float distance;

        m_faces = m_modelRenderer.lock()->getModel().lock()->getFaces();
        
        transform().lock()->updateMatrix();
        glm::mat4 modelMatrix = transform().lock()->getModelMatrix();

        for (int i = 0; i < m_faces->size(); i++)
        {
            glm::vec4 pa = modelMatrix * glm::vec4(m_faces->at(i).pa, 1.0f);
            glm::vec4 pb = modelMatrix * glm::vec4(m_faces->at(i).pb, 1.0f);
            glm::vec4 pc = modelMatrix * glm::vec4(m_faces->at(i).pc, 1.0f);

            if (glm::intersectRayTriangle(_ray.origin, _ray.direction,
                glm::vec3(pa.x, pa.y, pa.z), glm::vec3(pb.x, pb.y, pb.z), glm::vec3(pc.x, pc.y, pc.z), baryPosition, distance))
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
            info.intersectedModel = m_entity.lock()->findComponent<GameEngine::ModelRenderer>().lock()->getModel();
        
        return info;
    }

    void MeshCollider::updateOutline()
    {
        if (m_lineRendererDirty)
        {
            m_lineRendererDirty = false;

            // Attach line renderer
            std::vector<std::shared_ptr<GameEngine::LineRenderer> > lineRenderer;
            core().lock()->find<GameEngine::LineRenderer>(lineRenderer);
            m_lineRenderer = lineRenderer[0];

            // Create new vbo for this collider
            m_vbo = m_lineRenderer.lock()->addVbo();

            m_faces = m_modelRenderer.lock()->getModel().lock()->getFaces();

            transform().lock()->updateMatrix();
            glm::mat4 modelMatrix = transform().lock()->getModelMatrix();

            for (int i = 0; i < m_faces->size(); i++)
            {
                glm::vec4 pa = modelMatrix * glm::vec4(m_faces->at(i).pa, 1.0f);
                glm::vec4 pb = modelMatrix * glm::vec4(m_faces->at(i).pb, 1.0f);
                glm::vec4 pc = modelMatrix * glm::vec4(m_faces->at(i).pc, 1.0f);

                glm::vec3 pos1(pa.x, pa.y, pa.z);
                glm::vec3 pos2(pb.x, pb.y, pb.z);
                glm::vec3 pos3(pc.x, pc.y, pc.z);

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
