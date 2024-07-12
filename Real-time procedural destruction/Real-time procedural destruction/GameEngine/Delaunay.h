#pragma once
#include <glm/glm.hpp>
#include <vector>

namespace GameEngine
{
    struct Triangle
    {
        glm::vec2 vertices[3];

        bool circumcircleContains(const glm::vec2 _point)
        {
            float ax = vertices[0].x - _point.x;
            float ay = vertices[0].y - _point.y;

            float bx = vertices[1].x - _point.x;
            float by = vertices[1].y - _point.y;

            float cx = vertices[2].x - _point.x; 
            float cy = vertices[2].y - _point.y;


            float detab = ax * by - ay * bx;
            float detbc = bx * cy - by * cx;
            float detca = cx * ay - cy * ax;

            float aSquared = ax * ax + ay * ay;
            float bSquared = bx * bx + by * by;
            float cSquared = cx * cx + cy * cy;

            float determinant = aSquared * detbc + bSquared * detca + cSquared * detab;

            return determinant > 0.0f;
        }
    };

    struct Delaunay
    {
        Delaunay(const std::vector<glm::vec2>& _points);

    private:
        std::vector<Triangle> m_triangles;

        void addPoint(const glm::vec2& _point);
    };
}

