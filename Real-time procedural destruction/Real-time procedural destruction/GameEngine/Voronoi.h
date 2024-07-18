#pragma once
#include "Delaunay.h"

namespace GameEngine
{
    struct VoronoiCell
    {
        std::vector<Edge> m_edges;
        glm::vec2 m_site; // Center of the cell
    };

    struct VoronoiCell3D
    {
        std::vector<Edge3D> m_edges;
    };

    struct VoronoiDiagram
    {
        VoronoiDiagram();
        
        void generate(const std::vector<Triangle>& _delaunayTriangles);
        VoronoiCell generateConvexHull(const std::vector<VoronoiCell>& _cells);

        std::vector<VoronoiCell> m_voronoiCells;
    };
}

