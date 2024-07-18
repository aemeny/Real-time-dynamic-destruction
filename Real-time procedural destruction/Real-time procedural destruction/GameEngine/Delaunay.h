#pragma once
#include "Triangle.h"
#include <vector>

namespace GameEngine
{
    struct Delaunay
    {
        Delaunay(const std::vector<glm::vec2>& _points);
        Delaunay(const std::vector<glm::vec2>& _points, std::vector<Edge>& _constraints);

        std::vector<Triangle> m_triangles;
        std::vector<Edge> m_polygonEdges;
    private:
        Triangle m_superTriangle;
        void generateSuperTriangle(const std::vector<glm::vec2>& _points);
        void BowyerWatson(const std::vector<glm::vec2>& _points);
        void constrainedTriangulation(const std::vector<glm::vec2>& _meshPoints, const std::vector<glm::vec2>& _holePoints);
        std::vector<glm::vec2> getUniquePoints(std::vector<Edge>& _constraints);
        std::vector<glm::vec2> getUniquePoints(const std::vector<glm::vec2>& _points);
    };

    // Custom comparator for glm::vec2
    struct Vec2Comparator
    {
        bool operator()(const glm::vec2& a, const glm::vec2& b) const {
            if (a.x != b.x) return a.x < b.x;
            return a.y < b.y;
        }
    };
}

