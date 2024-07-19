#include "Delaunay.h"
#include <algorithm>
#include <map>
#include <set>

namespace GameEngine
{
    Delaunay::Delaunay(const std::vector<glm::vec2>& _points)
    {
        /*  Initialize the Triangulation :
           - Start with an empty triangulation.
           - Add a super - triangle that completely contains all the points in the point list.
        */
        generateSuperTriangle(_points);

        // triangulation using incremental insertion method, bowyer watson algorithm
        BowyerWatson(_points);
    }

    Delaunay::Delaunay(const std::vector<glm::vec2>& _points, std::vector<Edge>& _constraints)
    {
        // Get unique vertices from the mesh
        std::vector<glm::vec2> meshPoints = getUniquePoints(_points);

        // Generate Super Triangle to surround the points
        generateSuperTriangle(meshPoints);

        // Get unique vertices from the constraints
        std::vector<glm::vec2> orderedHolePoints = getEdgeUniquePoints(_constraints)[0];

        std::vector<glm::vec2> sharedPoint;
        sharedPoint.insert(sharedPoint.end(),
            meshPoints.begin(),
            meshPoints.end());
        sharedPoint.insert(sharedPoint.end(),
            orderedHolePoints.begin(),
            orderedHolePoints.end());

        // triangulation using incremental insertion method, bowyer watson algorithm
        BowyerWatson(sharedPoint);

        // Remove any triangles found in hole
        removeTrisInHole(orderedHolePoints, meshPoints);
    }

    std::vector<std::vector<glm::vec2>> Delaunay::getOrderedTrianglePoints(std::vector<Edge>& _outerEdges)
    {
        std::map<Edge, std::vector<const Triangle*>> edgeToTriangles;

        // Save each edge and triangles associated
        for (Triangle& tri : m_triangles) 
        {
            for (Edge& edge : tri.m_edges)
            {
                edgeToTriangles[edge].push_back(&tri);
            }
        }

        std::vector<Edge> hullEdges;
        for (const auto& [edge, triangles] : edgeToTriangles) 
        {
            if (triangles.size() == 1) // Only one triangle means it's an edge
            { 
                hullEdges.push_back(edge);
            }
        }

        return getEdgeUniquePoints(hullEdges);
    }

    void Delaunay::generateSuperTriangle(const std::vector<glm::vec2>& _points)
    {
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
                    polygonEdges.emplace_back(it->m_edges[0]);
                    polygonEdges.emplace_back(it->m_edges[1]);
                    polygonEdges.emplace_back(it->m_edges[2]);
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
                m_triangles.emplace_back(edge.m_start, edge.m_end, point);
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

    void Delaunay::removeTrisInHole(const std::vector<glm::vec2>& _holePoints, const std::vector<glm::vec2>& _meshPoints)
    {
        auto it = m_triangles.begin();
        while (it != m_triangles.end()) {
            // Calculate the centroid of the triangle
            glm::vec2 centroid = (it->m_vertices[0] + it->m_vertices[1] + it->m_vertices[2]) / 3.0f;

            // Check if the centroid is inside the hole polygon
            if (checkPointWithinPolygon(centroid, _holePoints))
            {
                it = m_triangles.erase(it); // Remove triangle
            }
            else
            {
                ++it;
            }
        }

        std::map<Edge, std::vector<const Triangle*>> edgeToTriangles;
        // Build the map from edges to triangles
        for (const Triangle& tri : m_triangles)
        {
            for (const Edge& edge : tri.m_edges)
            {
                edgeToTriangles[edge].push_back(&tri);
            }
        }
        // Identify triangles to keep
        std::set<const Triangle*> trianglesToKeep;
        for (const Triangle& tri : m_triangles)
        {
            int sharedEdgesCount = 0;
            for (const Edge& edge : tri.m_edges)
            {
                for (const glm::vec2& point : _meshPoints) //Check if its a corner tri
                {
                    if (edge.m_start == point || edge.m_end == point)
                    {
                        sharedEdgesCount++;
                    }
                }
                // Check if this edge is shared with another triangle
                if (edgeToTriangles[edge].size() > 1)
                {
                    sharedEdgesCount++;
                }
            }

            // Keep the triangle if at least two of its edges are shared
            if (sharedEdgesCount >= 2)
            {
                trianglesToKeep.insert(&tri);
            }
        }
        // Collect triangles that are adequately connected
        std::vector<Triangle> newTriangles;
        for (const Triangle& tri : m_triangles)
        {
            if (trianglesToKeep.find(&tri) != trianglesToKeep.end())
            {
                newTriangles.push_back(tri);
            }
        }

        // Replace old triangles with the filtered list
        m_triangles = std::move(newTriangles);
    }

    bool Delaunay::checkPointWithinPolygon(const glm::vec2& _point, const std::vector<glm::vec2>& _polygon)
    {
        int intersectCount = 0;
        for (size_t i = 0, j = _polygon.size() - 1; i < _polygon.size(); j = i++)
        {
            // Check if the line from (polygon[i] to polygon[j]) crosses the ray from 'point' to the right
            if (((_polygon[i].y > _point.y) != (_polygon[j].y > _point.y)) &&
                (_point.x < (_polygon[j].x - _polygon[i].x) * (_point.y - _polygon[i].y) / (_polygon[j].y - _polygon[i].y) + _polygon[i].x))
            {
                intersectCount++;
            }
        }
        // Odd count means the point is inside, even count means the point is outside
        return intersectCount % 2 == 1;
    }

    std::vector<std::vector<glm::vec2>> Delaunay::getEdgeUniquePoints(std::vector<Edge>& _constraints)
    {
        std::vector<std::vector<glm::vec2>> allLoops;
        std::set<int> usedEdges; // Using indices instead of actual edge objects for easier management

        while (usedEdges.size() != _constraints.size()) {
            std::vector<glm::vec2> orderedVertices;
            int currentEdgeIndex = -1;

            // Find an unused edge to start a new loop
            for (int i = 0; i < _constraints.size(); ++i) {
                if (usedEdges.find(i) == usedEdges.end()) {
                    currentEdgeIndex = i;
                    orderedVertices.push_back(_constraints[i].m_start);
                    break;
                }
            }

            if (currentEdgeIndex == -1) break; // No unused edge found, break the loop

            // Continue the loop from the found edge
            while (true) {
                usedEdges.insert(currentEdgeIndex);
                glm::vec2 currentVertex = _constraints[currentEdgeIndex].m_end;
                orderedVertices.push_back(currentVertex);

                // Find the next edge that contains 'currentVertex' and isn't used yet
                int nextEdgeIndex = -1; // Invalid index to indicate not found
                for (int i = 0; i < _constraints.size(); i++) {
                    if (usedEdges.find(i) != usedEdges.end()) continue;
                    if (_constraints[i].m_start == currentVertex || _constraints[i].m_end == currentVertex) {
                        nextEdgeIndex = i;
                        break;
                    }
                }

                if (nextEdgeIndex == -1 || _constraints[nextEdgeIndex].m_start == orderedVertices.front() && _constraints[nextEdgeIndex].m_end == orderedVertices[1]) {
                    // If no valid next edge is found or loops back to start, end current loop
                    break;
                }

                // Ensure the next edge starts where the last one ended
                if (_constraints[nextEdgeIndex].m_start != currentVertex) {
                    std::swap(_constraints[nextEdgeIndex].m_start, _constraints[nextEdgeIndex].m_end);
                }

                currentEdgeIndex = nextEdgeIndex;
            }

            if (!orderedVertices.empty()) {
                allLoops.push_back(orderedVertices);
            }
        }

        return allLoops;
    }

    std::vector<glm::vec2> Delaunay::getUniquePoints(const std::vector<glm::vec2>& _points)
    {
        std::map<glm::vec2, int, Vec2Comparator> mapOfUniquePoints;
        std::vector<glm::vec2> uniquePoints;

        // Add all vertices from model to map
        for (const glm::vec2& vertex : _points)
        {
            mapOfUniquePoints[vertex];
        }

        // Extract vertices from map to vector
        for (auto& vertex : mapOfUniquePoints)
        {
            uniquePoints.push_back(vertex.first);
        }

        return uniquePoints;
    }
}