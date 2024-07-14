#include "Delaunay.h"
#include <array>
#include <set>
#include <map>

namespace GameEngine
{
    Delaunay::Delaunay(const std::vector<glm::vec2>& _points)
    {
        /*  Initialize the Triangulation :
           - Start with an empty triangulation.
           - Add a super - triangle that completely contains all the points in the point list.
        */
        
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
        m_triangles.push_back(m_superTriangle);

        // triangulation using incremental insertion method, bowyer watson algorithm
        BowyerWatson(_points);
    }

    void Delaunay::BowyerWatson(const std::vector<glm::vec2>& _points) 
    {
        for (const glm::vec2& point : _points) // Add one point at a time
        {
            std::vector<Edge> polygonEdges;

            // Identify and remove all bad triangles
            auto it = m_triangles.begin();
            while (it != m_triangles.end()) {
                if (it->circumcircleContains(point)) {
                    // Save the edges of the 'bad' triangles before removing
                    Edge edges[3] = { // Grab edges
                    { it->m_vertices[0], it->m_vertices[1] },
                    { it->m_vertices[1], it->m_vertices[2] },
                    { it->m_vertices[2], it->m_vertices[0] }
                    };
                    for (const Edge& edge : edges) // Add to list of edges
                    {
                        polygonEdges.push_back(edge);
                    }
                    it = m_triangles.erase(it);  // Remove 'bad' triangle
                }
                else {
                    ++it;
                }
            }

            // Delete any duplicated / shared edges of the 'bad' triangles
            std::sort(polygonEdges.begin(), polygonEdges.end());
            std::vector<Edge> uniqueEdges;
            // As edges are  already sorted, any duplicates will be next to each other
            // simply check if the one infront is a duplicate and if so ignore and skip over these edges
            for (size_t i = 0; i < polygonEdges.size(); ++i) {
                if (i + 1 < polygonEdges.size() && polygonEdges[i] == polygonEdges[i + 1]) {
                    ++i;
                }
                else {
                    uniqueEdges.push_back(polygonEdges[i]); // Add non duplicates to a list
                }
            }
            polygonEdges.swap(uniqueEdges); // Swap duplicate list for new unique edges list


            // Re-triangulate the polygonal hole
            // Create a new triangle from all the edges and the point
            for (const Edge& edge : polygonEdges) {
                Triangle newTriangle(edge.m_start, edge.m_end, point);
                m_triangles.push_back(newTriangle);
            }
        }

        // Remove triangles that share vertices with the super triangle
        auto it = m_triangles.begin();
        while (it != m_triangles.end()) {
            if (it->containsVertex(m_superTriangle.m_vertices[0]) ||
                it->containsVertex(m_superTriangle.m_vertices[1]) ||
                it->containsVertex(m_superTriangle.m_vertices[2]))
            {
                it = m_triangles.erase(it);
            }
            else {
                ++it;
            }
        }
    }
}