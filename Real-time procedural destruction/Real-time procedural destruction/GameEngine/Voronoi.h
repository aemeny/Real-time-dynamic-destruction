#pragma once
#include <vector>
#include <glm/glm.hpp>

namespace GameEngine
{
    struct Edge
    {
        glm::vec2 m_start;
        glm::vec2 m_end;
    };

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

