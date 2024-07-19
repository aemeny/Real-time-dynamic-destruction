#pragma once
#include <glm/glm.hpp>
#include <vector>

namespace GameEngine 
{
    struct Edge;

    struct LineEquations
    {
        LineEquations();

        bool checkIntersect(const Edge& _this, const Edge& _other);
    private:
        bool isPointOnLine(const glm::vec2& point, const Edge& line);
    };

    struct Edge
    {
        glm::vec2 m_start;
        glm::vec2 m_end;
        int m_clipped;

        Edge();
        Edge(const glm::vec2& _start, const glm::vec2& _end);

        LineEquations lineEquation;
        bool operator<(const Edge& _other) const;
        bool operator==(const Edge& _other) const;
    };

    struct Edge3D // Just holds point data for when needed to translate into 3D space
    {
        glm::vec3 m_start;
        glm::vec3 m_end;
        Edge3D(const glm::vec3& _start, const glm::vec3& _end);
    };
}

