#pragma once
#include "Component.h"
#include "LineClippingAlgorithm.h"
#include "../Physics/BaseCollider.h"

namespace GameEngine
{
    struct DestructionHandler : Component
    {
        ~DestructionHandler();

        std::vector<VoronoiCell> destructObject(intersectionInfo* _info, std::weak_ptr<Transform> _transform);
        ProjectionPlane determineProjectionPlane(const bu::Face* _collidedFace);
    private:
        std::vector<glm::vec2> projectVertices(const std::vector<glm::vec3>* _points, ProjectionPlane _plane);
        std::vector<glm::vec3> generateSquarePoints(glm::vec3 _pos, float _size, int _depth, ProjectionPlane _plane, std::weak_ptr<Transform> _transform);
        
    };
}

