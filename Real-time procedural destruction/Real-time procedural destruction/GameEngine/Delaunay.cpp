#include "Delaunay.h"
#include <iostream>

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
        Triangle superTriangle = { {glm::vec2(midX - 2 * dMax, midY - dMax), glm::vec2(midX + 2 * dMax, midY - dMax), glm::vec2(midX, midY + 2 * dMax)} };
        m_triangles.push_back(superTriangle);

        // Loop each point for triangulation using incremental insertion method
        for (const glm::vec2& point : _points)
        {
            addPoint(point);
        }
    }

    void Delaunay::addPoint(const glm::vec2& _point)
    {
        std::vector<Triangle> newTriangles;

        for (Triangle& tri : m_triangles)
        {
            if (tri.circumcircleContains(_point))
            {
                
            }
            else
            {
                
            }
        }
    }
}