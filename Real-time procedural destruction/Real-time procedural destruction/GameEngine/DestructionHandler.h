#pragma once
#include "Component.h"
#include "LineClippingAlgorithm.h"
#include "../Physics/BaseCollider.h"

namespace GameEngine
{
    struct DestructionHandler : Component
    {
        ~DestructionHandler();

        void destructObject(intersectionInfo* _info, std::weak_ptr<Transform> _transform);
    private:
        ProjectionPlane determineProjectionPlane(const bu::Face* _collidedFace);
        std::vector<glm::vec2> projectVertices(const std::vector<glm::vec3>* _points, ProjectionPlane _plane, float& _savedPoint);
        glm::vec3 unProjectVertex(const glm::vec2& _point, ProjectionPlane _plane, float _savedPoint);
        
        std::vector<glm::vec3> generateSquarePoints(glm::vec3 _pos, float _size, int _depth, ProjectionPlane _plane, std::weak_ptr<Transform> _transform);
        std::vector<glm::vec3> generateCirclePoints(glm::vec3 _pos, float _size, int _depth, ProjectionPlane _plane, std::weak_ptr<Transform> _transform);
        std::vector<glm::vec2> modelVertices(const std::vector<bu::Face>* _faces, const std::weak_ptr<Transform>& _transform, ProjectionPlane _plane);
    };
}

