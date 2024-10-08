#include "Voronoi.h"
#include <map>
#include <algorithm>
#include <iostream>

namespace GameEngine
{
    VoronoiDiagram::VoronoiDiagram() {}

    void VoronoiDiagram::generate(const std::vector<Triangle>& _delaunayTriangles)
    {
        // Map to compared egdes with each triangle
        std::map<Edge, std::vector<const Triangle*>> edgeToTriangles;
        // Map to see how many cells each edge connects with
        std::map<glm::vec2, VoronoiCell, Vec2Comparator> voronoiCells;

        // Associate edges with triangles
        for (const auto& triangle : _delaunayTriangles) { 
            std::vector<Edge> edges = {
                Edge(triangle.m_vertices[0], triangle.m_vertices[1]),
                Edge(triangle.m_vertices[1], triangle.m_vertices[2]),
                Edge(triangle.m_vertices[2], triangle.m_vertices[0])
            };

            for (const Edge& edge : edges) {
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
                    if (vertex == edge.m_start || vertex == edge.m_end)
                        voronoiCells[vertex].m_edges.emplace_back(circum1, circum2);
                }
                for (const glm::vec2& vertex : triangles[1]->m_vertices) {
                    voronoiCells[vertex].m_site = vertex;
                    if (vertex == edge.m_start || vertex == edge.m_end)
                        voronoiCells[vertex].m_edges.emplace_back(circum2, circum1);
                }
            }
        }

        // Extract cells from map to vector
        for (auto& cellMap : voronoiCells) 
        {
            // Removes duplicate edges from edges being flipped
            std::sort(cellMap.second.m_edges.begin(), cellMap.second.m_edges.end());
            cellMap.second.m_edges.erase(std::unique(cellMap.second.m_edges.begin(), cellMap.second.m_edges.end()), cellMap.second.m_edges.end());

            // Add a dictionary of vertexes to check if two edges are touching, if not the cell isn't closed and can be disgarded
            std::map<glm::vec2, std::vector<const Edge*>, Vec2Comparator> connectingEdges;
            for (const Edge& edge : cellMap.second.m_edges)
            {
                connectingEdges[edge.m_start].push_back(&edge);
                connectingEdges[edge.m_end].push_back(&edge);
            }
            
            bool closedCell = true;
            for (const auto& [vertex, edges] : connectingEdges)
            {
                if (edges.size() == 1) // Only one edge is touching this vertex
                {
                    closedCell = false;
                }
            }
            if (closedCell)
            {
                m_voronoiCells.push_back(cellMap.second);
            }
        }
    }

    VoronoiCell VoronoiDiagram::generateConvexHull(const std::vector<VoronoiCell>& _cells)
    {
        std::map<Edge, std::vector<const VoronoiCell*>> edgeToCells;
        // Associate edges with cells
        for (const VoronoiCell& cell : _cells) 
        {
            for (const Edge& edge : cell.m_edges) 
            {
                edgeToCells[edge].push_back(&cell);
            }
        }

        // Create Voronoi Convex Hull from non connected edges
        VoronoiCell convexHull;
        for (const auto& [edge, cells] : edgeToCells) 
        {
            if (cells.size() == 1)  // Only consider edges connected to one cell
            {
                convexHull.m_edges.push_back(edge);
            }
        }

        return convexHull;
    }
}