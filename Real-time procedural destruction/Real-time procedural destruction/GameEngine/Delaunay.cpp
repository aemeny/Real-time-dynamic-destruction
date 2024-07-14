#include "Delaunay.h"
#include <iostream>
#include <array>
#include <set>
#include <map>

namespace GameEngine
{
    Delaunay::Delaunay(const std::vector<glm::vec2>& _points)
    {
        // Generate a bounding triangle or 'Super-Triangle' to encapsulate all points for initializing the Deluanay triangulation
        
        // Calculate the min and max values for both x and y values from the given points
        float minX = std::numeric_limits<float>::max();
        float maxX = std::numeric_limits<float>::min();
        float minY = std::numeric_limits<float>::max();
        float maxY = std::numeric_limits<float>::min();
        for (const glm::vec2& point : _points)
        {
            if (point.x < minX) minX = point.x;
            if (point.x > maxX) maxX = point.x;
            if (point.y < minY) minY = point.y;
            if (point.y > maxY) maxY = point.y;
        }

        // Use the found bounds to calculate the range / middle of these points
        float dx = maxX - minX;
        float dy = maxY - minY;
        float midX = (minX + maxX) * 0.5;
        float midY = (minY + maxY) * 0.5;

        // Multiply by three to make sure the triangle is always large enough
        float dMax = std::max(dx, dy) * 3;

        // Create the boudning triangle from the bounds and add to our triangles list
        m_superTriangle = Triangle(glm::vec2(midX - 2 * dMax, midY - dMax), glm::vec2(midX + 2 * dMax, midY - dMax), glm::vec2(midX, midY + 2 * dMax));

        // triangulation using incremental insertion method, bowyer watson algorithm
        BowyerWatson(_points);
    }

    void Delaunay::BowyerWatson(const std::vector<glm::vec2>& _points) {
        glm::vec2 p1(-1000, -1000), p2(1000, -1000), p3(0, 1000);  // Large super triangle
        Triangle superTriangle(p1, p2, p3);
        m_triangles.push_back(superTriangle);

        for (const auto& point : _points) {
            std::vector<Edge> polygonEdges;

            // Identify and remove all bad triangles
            auto it = m_triangles.begin();
            while (it != m_triangles.end()) {
                if (it->circumcircleContains(point)) {
                    Edge edges[3] = {
                        {it->vertices[0], it->vertices[1]},
                        {it->vertices[1], it->vertices[2]},
                        {it->vertices[2], it->vertices[0]}
                    };
                    for (const Edge& edge : edges) {
                        polygonEdges.push_back(edge);  // Collect edges of bad triangles
                    }
                    it = m_triangles.erase(it);  // Remove bad triangle
                }
                else {
                    ++it;
                }
            }

            // Remove duplicate edges
            std::sort(polygonEdges.begin(), polygonEdges.end());
            polygonEdges.erase(std::unique(polygonEdges.begin(), polygonEdges.end(),
                [](const Edge& a, const Edge& b) {
                    return (a == b) || (a.m_start == b.m_end && a.m_end == b.m_start);
                }), polygonEdges.end());

            // Re-triangulate the polygonal hole
            for (const Edge& edge : polygonEdges) {
                Triangle newTriangle(edge.m_start, edge.m_end, point);
                m_triangles.push_back(newTriangle);
            }
        }

        // Remove triangles that contain vertices of the super triangle
        m_triangles.erase(std::remove_if(m_triangles.begin(), m_triangles.end(),
            [&p1, &p2, &p3](const Triangle& tri) {
                return tri.containsVertex(p1) || tri.containsVertex(p2) || tri.containsVertex(p3);
            }), m_triangles.end());
    }
}