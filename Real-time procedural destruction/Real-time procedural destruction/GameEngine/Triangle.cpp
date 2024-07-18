#include "Triangle.h"

namespace GameEngine
{
    // Triangle //
    Triangle::Triangle() : m_vertices{ glm::vec2(0,0), glm::vec2(0,0), glm::vec2(0,0) },
        m_edges{ {glm::vec2(0,0), glm::vec2(0,0)}, {glm::vec2(0,0), glm::vec2(0,0)}, {glm::vec2(0,0), glm::vec2(0,0)} },
        m_circumcenter(0, 0), m_radius(0), m_circumcircleCalc(false), m_radiusCalc(false) {}

    Triangle::Triangle(const glm::vec2& _v1, const glm::vec2& _v2, const glm::vec2& _v3) {
        m_vertices[0] = _v1;
        m_vertices[1] = _v2;
        m_vertices[2] = _v3;
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