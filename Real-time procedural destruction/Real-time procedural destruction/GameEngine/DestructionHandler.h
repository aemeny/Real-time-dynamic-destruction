#pragma once
#include "Component.h"
#include "../Physics/BaseCollider.h"

namespace GameEngine
{
    enum ProjectionPlane {
        XY, YZ, XZ
    };

    struct DestructionHandler : Component
    {
        ~DestructionHandler();

        std::vector<glm::vec3> destructObject(intersectionInfo* _info);
    private:
        std::vector<glm::vec2> projectVertices(const std::vector<bu::Face>* _faces, ProjectionPlane _plane);
        ProjectionPlane determineProjectionPlane(const bu::Face* _collidedFace);
        std::vector<glm::vec3> generateCubePoints(glm::vec3 _pos, float _size, int _depth);
        
    };
}

