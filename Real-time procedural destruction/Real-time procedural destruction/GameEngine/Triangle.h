#pragma once
#include <glm/glm.hpp>

namespace GameEngine
{
    struct Edge
    {
        glm::vec2 m_start;
        glm::vec2 m_end;
        int m_clipped;

        Edge();
        Edge(const glm::vec2& _start, const glm::vec2& _end);

        bool operator<(const Edge& _other) const;
        bool operator==(const Edge& _other) const;
    };

    struct Edge3D // Just holds point data for when needed to translate into 3D space
    {
        glm::vec3 m_start;
        glm::vec3 m_end;
        Edge3D(const glm::vec3& _start, const glm::vec3& _end);
    };

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

