#pragma once
#include <glm/glm.hpp>
#include <vector>

namespace GameEngine 
{
    struct Edge;

    struct LineEquations
    {
        LineEquations();

        // Checks if two egdes intersect at any point
        bool checkIntersect(const Edge& _this, const Edge& _other);
    };

    struct Edge
    {
        // Start and end point of the edge
        glm::vec2 m_start;
        glm::vec2 m_end;
        // If the edge has been clipped, and which point
        int m_clipped;

        Edge();
        Edge(const glm::vec2& _start, const glm::vec2& _end);

        LineEquations lineEquation; // Each edge has their own line equation if needed
        bool operator<(const Edge& _other) const; // Overload operators for custom comparason
        bool operator==(const Edge& _other) const;
    };

    struct Edge3D // Just holds point data for when needed to translate into 3D space
    {
        glm::vec3 m_start;
        glm::vec3 m_end;
        Edge3D(const glm::vec3& _start, const glm::vec3& _end);
    };
}

