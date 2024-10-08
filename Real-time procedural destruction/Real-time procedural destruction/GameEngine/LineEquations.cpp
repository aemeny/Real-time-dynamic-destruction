#include "LineEquations.h"

namespace GameEngine
{
    // EDGE //
    Edge::Edge() : m_start(glm::vec2(0)), m_end(glm::vec2(0)), m_clipped(0)
    {
        lineEquation = LineEquations();
    }

    Edge::Edge(const glm::vec2& _start, const glm::vec2& _end)
    {
        m_clipped = 0;
        lineEquation = LineEquations();
        // Compare first by x-coordinate, then by y-coordinate to ensure consistent ordering
        if (_start.x < _end.x || (_start.x == _end.x && _start.y < _end.y))
        {
            m_start = _start;
            m_end = _end;
        }
        else
        {
            m_start = _end;
            m_end = _start;
        }
    }
    bool Edge::operator<(const Edge& _other) const
    {
        // Use the same ordering logic as used in the constructor for comparison
        return m_start.x < _other.m_start.x ||
            (m_start.x == _other.m_start.x && m_start.y < _other.m_start.y) ||
            (m_start.x == _other.m_start.x && m_start.y == _other.m_start.y && m_end.x < _other.m_end.x) ||
            (m_start.x == _other.m_start.x && m_start.y == _other.m_start.y && m_end.x == _other.m_end.x && m_end.y < _other.m_end.y);
    }
    bool Edge::operator==(const Edge& _other) const
    {
        // Equality comparison to consider reversed edges
        return (m_start == _other.m_start && m_end == _other.m_end) || (m_start == _other.m_end && m_end == _other.m_start);
    }


     // EDGE 3D //
    Edge3D::Edge3D(const glm::vec3& _start, const glm::vec3& _end) :
        m_start(_start), m_end(_end) {}


    // LINE EQUATIONS //
    LineEquations::LineEquations() {}

    bool LineEquations::checkIntersect(const Edge& _this, const Edge& _other)
    {
        // Direction vectors for the edges
        glm::vec2 d1 = _this.m_end - _this.m_start;
        glm::vec2 d2 = _other.m_end - _other.m_start;

        // Perpendicular vector for other edge
        glm::vec2 perp2(d2.y, -d2.x);
        // Determinant
        float det = glm::dot(d1, perp2);

        if (std::fabs(det) < 1e-6) // Lines are parallel, no intersection (with floating point errors)
            return false; 

        // Dot product
        glm::vec2 diff = _other.m_start - _this.m_start;
        float t = glm::dot(diff, perp2) / det;
        if (t < 0 || t > 1) // Intersection not along the segment of the first edge
            return false; 

        // Perpendicular vector for this edge
        glm::vec2 perp1(-d1.y, d1.x);
        float u = glm::dot(diff, perp1) / det;

        // Check if the intersection point is along the segment of the second edge
        return u >= 0 && u <= 1; 
    }
}