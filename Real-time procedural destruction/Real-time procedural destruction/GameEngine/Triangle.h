#pragma once
#include "LineEquations.h"

namespace GameEngine
{
    struct Triangle
    {
        glm::vec2 m_vertices[3];
        Edge m_edges[3];

        Triangle();
        Triangle(const glm::vec2& _v1, const glm::vec2& _v2, const glm::vec2& _v3);

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

