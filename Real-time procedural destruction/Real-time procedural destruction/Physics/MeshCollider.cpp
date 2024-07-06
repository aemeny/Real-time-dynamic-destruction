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

    /*
    * onDisplay() runs every frame after onTick()
    */
    void MeshCollider::onDisplay()
    {
        if (m_renderOutline)
            m_lineRenderer.lock()->renderLine(transform().lock()->getModelMatrix());
    }

    bool MeshCollider::preTransformVertices() 
    {
        if (transform().lock()->getDirty() == false) // If the models transform or faces havent been changed no need to update
            return false;

        transform().lock()->updateMatrix();
        glm::mat4 modelMatrix = transform().lock()->getModelMatrix();
        std::vector<bu::Face>* faces = m_modelRenderer.lock()->getModel().lock()->getFaces();

        // Reset vector of vertices for new data
        m_transformedVertices.clear();
        m_transformedVertices.shrink_to_fit();
        m_transformedVertices.reserve(faces->size() * 3);

        // loop through each face and mutiply the vertex position by the model matrix to get true world position
        for (const bu::Face& face : *faces) 
        {
            m_transformedVertices.push_back(modelMatrix * glm::vec4(face.pa, 1.0f));
            m_transformedVertices.push_back(modelMatrix * glm::vec4(face.pb, 1.0f));
            m_transformedVertices.push_back(modelMatrix * glm::vec4(face.pc, 1.0f));
        }

        // Turn dirty flag off
        transform().lock()->setDirty(false);

        return true; // Data has been updated
    }

    intersectionInfo MeshCollider::rayIntersect(Ray _ray)
    {
        intersectionInfo info;

        float nearest = -1.0f;
        glm::vec2 baryPosition = glm::vec2(0); // Local 2D position collided on tri
        float distance; // Distance from ray origin to tri

        // If face locations are dirty, update
        preTransformVertices(); 

        int index = -1; // Collided vertex index
        // Loop through all faces (3 vertices per face)
        for (int i = 0; i < m_transformedVertices.size(); i += 3)
        {
            if (glm::intersectRayTriangle(_ray.origin, _ray.direction,
                glm::vec3(m_transformedVertices[i].x,     m_transformedVertices[i].y,     m_transformedVertices[i].z), 
                glm::vec3(m_transformedVertices[i + 1].x, m_transformedVertices[i + 1].y, m_transformedVertices[i + 1].z), 
                glm::vec3(m_transformedVertices[i + 2].x, m_transformedVertices[i + 2].y, m_transformedVertices[i + 2].z),
                baryPosition, distance))
            {
                if (distance > 0.0f) // If intersect + distance is infront
                {
                    if (nearest == -1 || distance < nearest) // If new closest tri intersected set info
                    {
                        nearest = distance;
                        info.hasIntersected = true;
                        info.distanceToIntersection = distance;
                        index = i / 3; // Divide by three to get face index rather than vertices (3 Vertice per face)
                    }
                }
            }
        }

        if (info.hasIntersected) // If any collision, pull data to save
        {
            std::vector<bu::Face>* faces = m_modelRenderer.lock()->getModel().lock()->getFaces();
            info.collidedFace = &faces->at(index);
            info.intersectedModel = m_entity.lock()->findComponent<GameEngine::ModelRenderer>().lock()->getModel();

            float d = info.distanceToIntersection;
            info.intersectionPos = glm::vec3(_ray.origin.x + (_ray.direction.x * d), _ray.origin.y + (_ray.direction.y * d), _ray.origin.z + (_ray.direction.z * d));
        }

        return info;
    }

    void MeshCollider::updateOutline()
    {
        if (preTransformVertices()) // If faces currently being displayed are dirty
        {
            if (m_vbo.expired()) // if vbo hasnt been init yet
            {
                // Attach line renderer
                std::vector<std::shared_ptr<GameEngine::LineRenderer> > lineRenderer;
                core().lock()->find<GameEngine::LineRenderer>(lineRenderer);
                m_lineRenderer = lineRenderer[0];

                // Create new vbo for this collider
                m_vbo = m_lineRenderer.lock()->addVbo();
            }

            // Clear the already made lines from line count + clear all data from vbo list
            m_lineRenderer.lock()->clearLines(m_vbo); 

            // Loop through all faces (3 vertices per face) and create a line for each edge of the tri
            for (int i = 0; i < m_transformedVertices.size(); i += 3)
            {
                glm::vec3 pos1(m_transformedVertices[i].x, m_transformedVertices[i].y, m_transformedVertices[i].z);
                glm::vec3 pos2(m_transformedVertices[i + 1].x, m_transformedVertices[i + 1].y, m_transformedVertices[i + 1].z);
                glm::vec3 pos3(m_transformedVertices[i + 2].x, m_transformedVertices[i + 2].y, m_transformedVertices[i + 2].z);

                m_lineRenderer.lock()->addLine(m_vbo, pos1, pos2);
                m_lineRenderer.lock()->addLine(m_vbo, pos2, pos3);
                m_lineRenderer.lock()->addLine(m_vbo, pos3, pos1);
            }
        }
    }
}
