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

    // Custom comparator for glm::vec2
    struct Vec2Comparator 
    {
        bool operator()(const glm::vec2& a, const glm::vec2& b) const {
            if (a.x != b.x) return a.x < b.x;
            return a.y < b.y;
        }
    };
}

