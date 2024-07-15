#include "Voronoi.h"
#include <map>
#include <algorithm>

namespace GameEngine
{
    VoronoiDiagram::VoronoiDiagram() {}

    void VoronoiDiagram::generate(const std::vector<Triangle>& _delaunayTriangles)
    {
        std::map<Edge, std::vector<const Triangle*>> edgeToTriangles;
        std::map<glm::vec2, VoronoiCell, Vec2Comparator> voronoiCells;

        // Associate edges with triangles
        for (const auto& triangle : _delaunayTriangles) {
            std::vector<Edge> edges = {
                Edge(triangle.m_vertices[0], triangle.m_vertices[1]),
                Edge(triangle.m_vertices[1], triangle.m_vertices[2]),
                Edge(triangle.m_vertices[2], triangle.m_vertices[0])
            };

            for (const auto& edge : edges) {
                edgeToTriangles[edge].push_back(&triangle);
            }
        }

        // Create Voronoi edges by connecting circumcenters of adjacent triangles
        for (const auto& [edge, triangles] : edgeToTriangles) {
            if (triangles.size() == 2) { // Only consider edges shared by exactly two triangles
                glm::vec2 circum1 = triangles[0]->getCircumcenter();
                glm::vec2 circum2 = triangles[1]->getCircumcenter();

                // Add Voronoi edge to each triangle's vertex Voronoi cell
                for (const glm::vec2& vertex : triangles[0]->m_vertices) {
                    voronoiCells[vertex].m_site = vertex;
                    voronoiCells[vertex].m_edges.emplace_back(circum1, circum2);
                }
                for (const glm::vec2& vertex : triangles[1]->m_vertices) {
                    voronoiCells[vertex].m_site = vertex;
                    voronoiCells[vertex].m_edges.emplace_back(circum2, circum1);
                }
            }
        }

        // Extract cells from map to vector
        for (const auto& cellMap : voronoiCells) 
        {
            m_voronoiCells.push_back(cellMap.second);
        }
    }
    
}