#include "Triangle.h"

namespace GameEngine
{
    // EDGE //
    Edge::Edge() : m_start(glm::vec2(0)), m_end(glm::vec2(0)), m_clipped(0) {}
    
    Edge::Edge(const glm::vec2& _start, const glm::vec2& _end) 
    {
        m_clipped = 0;
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


    // Triangle //
    Triangle::Triangle() : m_vertices{ glm::vec2(0,0), glm::vec2(0,0), glm::vec2(0,0) },
        m_edges{ {glm::vec2(0,0), glm::vec2(0,0)}, {glm::vec2(0,0), glm::vec2(0,0)}, {glm::vec2(0,0), glm::vec2(0,0)} },
        m_circumcenter(0, 0), m_radius(0), m_circumcircleCalc(false), m_radiusCalc(false) {}

    Triangle::Triangle(const glm::vec2& v1, const glm::vec2& v2, const glm::vec2& v3) {
        m_vertices[0] = v1;
        m_vertices[1] = v2;
        m_vertices[2] = v3;
        m_edges[0] = { m_vertices[0], m_vertices[1] };
        m_edges[1] = { m_vertices[1], m_vertices[2] };
        m_edges[2] = { m_vertices[2], m_vertices[0] };
        m_circumcircleCalc = false;
        m_radiusCalc = false;
    }

    bool Triangle::circumcircleContains(const glm::vec2& _point)
    {
        if (!m_circumcircleCalc)
        {
            getCircumcenter();
        }
        if (!m_radiusCalc)
        {
            getRadius();
        }
        return glm::distance(_point, m_circumcenter) < m_radius;
    }

    glm::vec2 Triangle::getCircumcenter() const
    {
        if (!m_circumcircleCalc)
        {
            m_circumcircleCalc = true;

            double ax = m_vertices[0].x;
            double ay = m_vertices[0].y;
            double bx = m_vertices[1].x;
            double by = m_vertices[1].y;
            double cx = m_vertices[2].x;
            double cy = m_vertices[2].y;

            double d = 2 * (ax * (by - cy) + bx * (cy - ay) + cx * (ay - by));
            double ux = ((ax * ax + ay * ay) * (by - cy) + (bx * bx + by * by) * (cy - ay) + (cx * cx + cy * cy) * (ay - by)) / d;
            double uy = ((ax * ax + ay * ay) * (cx - bx) + (bx * bx + by * by) * (ax - cx) + (cx * cx + cy * cy) * (bx - ax)) / d;

            m_circumcenter = glm::vec2(ux, uy);
        }
        return m_circumcenter;
    }

    bool Triangle::containsVertex(const glm::vec2& _otherVertex) const
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

    float Triangle::getRadius() const
    {
        if (!m_radiusCalc)
        {
            m_radiusCalc = true;
            m_radius = glm::distance(m_circumcenter, m_vertices[0]);  // Euclidean distance
        }
        return m_radius;
    }

}