#pragma once
#include "Triangle.h"
#include <vector>

namespace GameEngine
{
    struct Delaunay
    {
        Delaunay(const std::vector<glm::vec2>& _points);

        std::vector<Triangle> m_triangles;
    private:
        Triangle m_superTriangle;
        void BowyerWatson(const std::vector<glm::vec2>& _points);
    };
}

