#pragma once
#include <glm/glm.hpp>

namespace GameEngine
{
    struct Edge
    {
        glm::vec2 m_start;
        glm::vec2 m_end;

        Edge();
        Edge(const glm::vec2& start, const glm::vec2& end);

        bool operator<(const Edge& other) const;
        bool operator==(const Edge& other) const;
    };

    struct Triangle
    {
        glm::vec2 m_vertices[3];
        Edge m_edges[3];

        Triangle();
        Triangle(const glm::vec2& v1, const glm::vec2& v2, const glm::vec2& v3);

        bool circumcircleContains(const glm::vec2& _point);
        glm::vec2 getCircumcenter() const;

        bool containsVertex(const glm::vec2& _otherVertex) const;

    private:
        mutable glm::vec2 m_circumcenter;
        mutable float m_radius;
        mutable bool m_circumcircleCalc;
        mutable bool m_radiusCalc;

        float getRadius() const;
    };

}

