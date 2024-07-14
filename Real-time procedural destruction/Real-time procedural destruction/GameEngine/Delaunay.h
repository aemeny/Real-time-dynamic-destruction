#pragma once
#include <vector>
#include <set>
#include <algorithm>
#include <glm/glm.hpp>
#include <limits>
#include <cmath>
#include <tuple>

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

        bool operator==(const Edge& other) const {
            return m_start == other.m_start && m_end == other.m_end;
        }
    };

    struct Triangle
    {
        glm::vec2 vertices[3];
        mutable glm::vec2 circumcenter;
        mutable float radius;
        mutable bool circumcenterCalculated;

        Triangle() : vertices{ glm::vec2(0,0), glm::vec2(0,0), glm::vec2(0,0) },
            circumcenter(0, 0),
            radius(0),
            circumcenterCalculated(false) {}

        Triangle(const glm::vec2& v1, const glm::vec2& v2, const glm::vec2& v3) {
            vertices[0] = v1;
            vertices[1] = v2;
            vertices[2] = v3;
            circumcenterCalculated = false;
        }

        bool operator<(const Triangle& other) const {
            // Simple lexicographical ordering based on vertices
            for (int i = 0; i < 3; i++) {
                if (vertices[i].x < other.vertices[i].x) return true;
                if (vertices[i].x > other.vertices[i].x) return false;
                if (vertices[i].y < other.vertices[i].y) return true;
                if (vertices[i].y > other.vertices[i].y) return false;
            }
            return false; // They are equal if all coordinates match
        }

        bool circumcircleContains(const glm::vec2 _point)
        {
            if (!circumcenterCalculated) 
            {
                circumcenter = getCircumcenter();
                radius = glm::distance(circumcenter, vertices[0]);  // Euclidean distance
                circumcenterCalculated = true;
            }
            float dist = glm::distance(_point, circumcenter);
            return dist < radius * (1.0f - 1e-6f); // Adjust epsilon as needed
        }

        glm::vec2 getCircumcenter() const 
        {
            float ax = vertices[0].x;
            float ay = vertices[0].y;
            float bx = vertices[1].x;
            float by = vertices[1].y;
            float cx = vertices[2].x;
            float cy = vertices[2].y;
            
            float d = 2 * (ax * (by - cy) + bx * (cy - ay) + cx * (ay - by));
            if (std::fabs(d) < 1e-6)
                return glm::vec2(0, 0); // Handle collinear case or return a default point
            
            float ux = ((ax * ax + ay * ay) * (by - cy) + (bx * bx + by * by) * (cy - ay) + (cx * cx + cy * cy) * (ay - by)) / d;
            float uy = ((ax * ax + ay * ay) * (cx - bx) + (bx * bx + by * by) * (ax - cx) + (cx * cx + cy * cy) * (bx - ax)) / d;

            return glm::vec2(ux, uy);
        }

        bool containsVertex(const glm::vec2& _point) const {
            const float EPSILON = 1e-6f;
            for (const glm::vec2& v : vertices) {
                if (std::fabs(v.x - _point.x) < EPSILON && std::fabs(v.y - _point.y) < EPSILON) {
                    return true;  // Return immediately when a match is found
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

