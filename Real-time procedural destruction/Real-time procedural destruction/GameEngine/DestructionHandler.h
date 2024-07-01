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

        void destructObject(intersectionInfo* _info);
    private:
        std::vector<glm::vec2> projectVertices(const std::vector<bu::Face>* _faces, ProjectionPlane _plane);
        ProjectionPlane determineProjectionPlane(const bu::Face* _collidedFace);
        void injectVertices(std::vector<bu::Face>* _faces, ProjectionPlane _plane);
    };
}

