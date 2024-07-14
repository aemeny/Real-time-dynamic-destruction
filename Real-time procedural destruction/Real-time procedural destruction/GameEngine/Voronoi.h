#pragma once
#include "Delaunay.h"

namespace GameEngine
{
    struct VoronoiCell
    {
        std::vector<Edge> m_edges;
    };

    struct VoronoiDiagram
    {
        VoronoiDiagram();
        ~VoronoiDiagram();
        
        std::vector<VoronoiCell> generate(const glm::vec2& _impactPosition, const std::vector<glm::vec2>& _boundaryPositions);
    };
}

