#pragma once
#include <vector>
#include <set>
#include <algorithm>
#include <glm/glm.hpp>
#include <limits>
#include <cmath>
#include <tuple>
#include <iostream>

namespace GameEngine
{
    struct Edge
    {
        glm::vec2 m_start;
        glm::vec2 m_end;

        Edge(const glm::vec2& start, const glm::vec2& end) {
            // Compare first by x-coordinate, then by y-coordinate to ensure consistent ordering
            if (start.x < end.x || (start.x == end.x && start.y < end.y)) {
                m_start = start;
                m_end = end;
            }
            else {
                m_start = end;
                m_end = start;
            }
        }
        
        bool operator<(const Edge& other) const {
            // Use the same ordering logic as used in the constructor for comparison
            return m_start.x < other.m_start.x ||
                (m_start.x == other.m_start.x && m_start.y < other.m_start.y) ||
                (m_start.x == other.m_start.x && m_start.y == other.m_start.y && m_end.x < other.m_end.x) ||
                (m_start.x == other.m_start.x && m_start.y == other.m_start.y && m_end.x == other.m_end.x && m_end.y < other.m_end.y);
        }

        // Equality comparison to consider reversed edges as the same
        bool operator==(const Edge& other) const {
            return (m_start == other.m_start && m_end == other.m_end) || (m_start == other.m_end && m_end == other.m_start);
        }
    };

    struct Triangle
    {
        glm::vec2 m_vertices[3];
        mutable glm::vec2 m_circumcenter;
        mutable float m_radius;
        mutable bool m_circumcircleCalculated;

        Triangle() : m_vertices{glm::vec2(0,0), glm::vec2(0,0), glm::vec2(0,0)},
            m_circumcenter(0, 0), m_radius(0), m_circumcircleCalculated(false) {}

        Triangle(const glm::vec2& v1, const glm::vec2& v2, const glm::vec2& v3) {
            m_vertices[0] = v1;
            m_vertices[1] = v2;
            m_vertices[2] = v3;
            m_circumcircleCalculated = false;
        }

        bool circumcircleContains(const glm::vec2& _point)
        {
            if (!m_circumcircleCalculated)
            {
                m_circumcenter = getCircumcenter();
                m_radius = glm::distance(m_circumcenter, m_vertices[0]);  // Euclidean distance
                m_circumcircleCalculated = true;
            }
            float dis = glm::distance(_point, m_circumcenter);
            return glm::distance(_point, m_circumcenter) < m_radius;
        }

        glm::vec2 getCircumcenter() const 
        {
            double ax = m_vertices[0].x;
            double ay = m_vertices[0].y;
            double bx = m_vertices[1].x;
            double by = m_vertices[1].y;
            double cx = m_vertices[2].x;
            double cy = m_vertices[2].y;
            
            double d = 2 * (ax * (by - cy) + bx * (cy - ay) + cx * (ay - by));
            double ux = ((ax * ax + ay * ay) * (by - cy) + (bx * bx + by * by) * (cy - ay) + (cx * cx + cy * cy) * (ay - by)) / d;
            double uy = ((ax * ax + ay * ay) * (cx - bx) + (bx * bx + by * by) * (ax - cx) + (cx * cx + cy * cy) * (bx - ax)) / d;

            return glm::vec2(ux, uy);
        }

        bool containsVertex(const glm::vec2& _otherVertex) const 
        {
            for (const glm::vec2& vertex : m_vertices)
            {
                if (vertex == _otherVertex)
                {
                    return true;
                }
            }
            return false;
        }
    };

    struct Delaunay
    {
        Delaunay(const std::vector<glm::vec2>& _points);

        std::vector<Triangle> m_triangles;
    private:
        Triangle m_superTriangle;
        void BowyerWatson(const std::vector<glm::vec2>& _points);
    };
}

