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
        // Get unique vertices from the constraints
        std::vector<glm::vec2> meshPoints = getUniquePoints(_points);

        // Generate Super Triangle to surround the points
        generateSuperTriangle(meshPoints);

        // triangulation using incremental insertion method, bowyer watson algorithm
        BowyerWatson(meshPoints);

        std::vector<glm::vec2> holePoints = getUniquePoints(_constraints);

        m_polygonEdges.clear();
        for (size_t i = 0; i < holePoints.size(); ++i) {
            size_t nextIndex = (i + 1) % holePoints.size();  // Wraps around to the first vertex
            m_polygonEdges.push_back(Edge(holePoints[i], holePoints[nextIndex]));
        }

        // Add constraints and recalculate
        constrainedTriangulation(meshPoints, holePoints);
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

    void Delaunay::constrainedTriangulation(const std::vector<glm::vec2>& _meshPoints, const std::vector<glm::vec2>& _holePoints)
    {
        std::vector<Edge> polygonEdges;

        //for (const Edge& constraint : _constraints) {
        //    auto it = m_triangles.begin();
        //    while (it != m_triangles.end()) {
        //        bool intersects = false;

        //        // Iterate over the array of edges
        //        for (int i = 0; i < 3; ++i) {
        //            if (it->m_edges[i].lineEquation.checkIntersect(it->m_edges[i], constraint)) {
        //                intersects = true;
        //                break; // No need to check further edges if one intersects
        //            }
        //        }

        //        if (intersects) {
        //            // If any edge of the triangle intersects the constraint, remove the triangle
        //            polygonEdges.insert(polygonEdges.end(), std::begin(it->m_edges), std::end(it->m_edges));
        //            it = m_triangles.erase(it);
        //        }
        //        else {
        //            // If no edges intersect, keep all edges to later close the polygonal hole
        //            ++it;
        //        }
        //    }

        //    // Delete any duplicated / shared edges of the 'bad' triangles
        //    std::sort(polygonEdges.begin(), polygonEdges.end());
        //    std::vector<Edge> uniqueEdges;
        //    // As edges are  already sorted, any duplicates will be next to each other
        //    // simply check if the one infront is a duplicate and if so ignore and skip over these edges
        //    for (size_t i = 0; i < polygonEdges.size(); ++i) {
        //        if (i + 1 < polygonEdges.size() && polygonEdges[i] == polygonEdges[i + 1]) {
        //            ++i;
        //        }
        //        else {
        //            uniqueEdges.push_back(polygonEdges[i]); // Add non duplicates to a list
        //        }
        //    }
        //    polygonEdges.swap(uniqueEdges); // Swap duplicate list for new unique edges list
        //}

        //for (const Triangle& tri : m_triangles)
        //{
        //    polygonEdges.push_back(tri.m_edges[0]);
        //    polygonEdges.push_back(tri.m_edges[1]);
        //    polygonEdges.push_back(tri.m_edges[2]);
        //}

        for (int i = 0; i < _holePoints.size(); i++)
        {
            
        }

        m_polygonEdges = polygonEdges;
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

    std::vector<glm::vec2> Delaunay::getUniquePoints(std::vector<Edge>& _constraints)
    {
        std::vector<glm::vec2> orderedVertices;
        std::set<int> usedEdges; // Using indices instead of actual edge objects for easier management
        int currentEdgeIndex = 0; // Start with the first edge in the list
        orderedVertices.push_back(_constraints[currentEdgeIndex].m_start);

        while (true) 
        {
            usedEdges.insert(currentEdgeIndex);
            glm::vec2 currentVertex = _constraints[currentEdgeIndex].m_end;
            orderedVertices.push_back(currentVertex);

            // Find the next edge that contains 'currentVertex' and isn't used yet
            int nextEdgeIndex = _constraints.size(); // Invalid index to indicate not found
            for (int i = 0; i < _constraints.size(); i++)
            {
                if (usedEdges.find(i) != usedEdges.end()) continue;
                if (_constraints[i].m_start == currentVertex || _constraints[i].m_end == currentVertex) 
                {
                    nextEdgeIndex = i;
                    break;
                }
            }

            if (nextEdgeIndex == _constraints.size() || nextEdgeIndex == 0)
                break; // Break if no valid next edge is found or if we looped back to the start

            // Ensure the next edge starts where the last one ended
            if (_constraints[nextEdgeIndex].m_start != currentVertex) 
            {
                std::swap(_constraints[nextEdgeIndex].m_start, _constraints[nextEdgeIndex].m_end);
            }

            currentEdgeIndex = nextEdgeIndex;
        }

        return orderedVertices;
    }
}