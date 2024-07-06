#include "DestructionHandler.h"
#include "../Physics/TraceRay.h"

#include <random>

namespace GameEngine
{
    DestructionHandler::~DestructionHandler() {}

    std::vector<glm::vec3> DestructionHandler::destructObject(intersectionInfo* _info)
    {
        // Grab faces of model
        std::vector<bu::Face>* faces = _info->intersectedModel.lock()->getFaces();

        // Use normal vector of the collided face to determine projection
        //ProjectionPlane plane = determineProjectionPlane(_info->collidedFace);

        // Project faces into 2D depending on normal collided with
        //std::vector<glm::vec2> projectedVertices = projectVertices(faces, plane);

        return generateCubePoints(_info->intersectionPos, 3.5f, 100);
       

        // Update model
        core().lock()->m_traceRay->getObjectsInScene()->at(_info->objIndex)->transform().lock()->setDirty(true);
        _info->intersectedModel.lock()->updateModel();
    }

    std::vector<glm::vec2> DestructionHandler::projectVertices(const std::vector<bu::Face>* _faces, ProjectionPlane _plane)
    {
        std::vector<glm::vec2> projectedVertices;
        projectedVertices.reserve(_faces->size() * 3);
        switch (_plane) // Switchs which plane I need to project the models 3D vertices to 2D
        {
        case XY: // Cuts the z axis
            for (const bu::Face& face : *_faces)
            {
                projectedVertices.emplace_back(face.pa.x, face.pa.y);
                projectedVertices.emplace_back(face.pb.x, face.pb.y);
                projectedVertices.emplace_back(face.pc.x, face.pc.y);
            }
        break;
        case YZ: // Cuts the x axis
            for (const bu::Face& face : *_faces)
            {
                projectedVertices.emplace_back(face.pa.y, face.pa.z);
                projectedVertices.emplace_back(face.pb.y, face.pb.z);
                projectedVertices.emplace_back(face.pc.y, face.pc.z);
            }
        break;
        case XZ: // Cuts the y axis
            for (const bu::Face& face : *_faces)
            {
                projectedVertices.emplace_back(face.pa.x, face.pa.z);
                projectedVertices.emplace_back(face.pb.x, face.pb.z);
                projectedVertices.emplace_back(face.pc.x, face.pc.z);
            }
        break;
        }
        return projectedVertices;
    }

    ProjectionPlane DestructionHandler::determineProjectionPlane(const bu::Face* _collidedFace)
    {
        //Find absoulate value of each normal
        float absX = std::abs(_collidedFace->na.x);
        float absY = std::abs(_collidedFace->na.y);
        float absZ = std::abs(_collidedFace->na.z);

        // Find which normal is largest
        if (absX > absY && absX > absZ) 
        {
            return ProjectionPlane::YZ;  // Normal is aligned to the x axis, project onto YZ plane
        }
        else if (absY > absZ) 
        {
            return ProjectionPlane::XZ;  // Normal is aligned to the y axis, project onto XZ plane
        }
        else 
        {
            return ProjectionPlane::XY;  // Normal is aligned to the z axis, project onto XY plane
        }
    }

    std::vector<glm::vec3> DestructionHandler::generateCubePoints(glm::vec3 _pos, float _size, int _depth)
    {
        std::vector<glm::vec3> points;
        for (int i = 0; i < _depth; i++)
        {
            float rand1 = ((float)rand() / (RAND_MAX)) * 2 - 1;
            float rand2 = ((float)rand() / (RAND_MAX)) * 2 - 1;
            float rand3 = ((float)rand() / (RAND_MAX)) * 2 - 1;
            points.push_back(glm::vec3(_pos.x + ((rand1 * _size) * 0.5f), _pos.y + ((rand2 * _size) * 0.5f), _pos.z + ((rand3 * _size) * 0.5f)));
        }
        return points;
    }
}