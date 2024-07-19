#pragma once
#include "Triangle.h"

namespace GameEngine
{
    //!  Delaunay class. 
    /*!
      Delaunay class triangulats given points to the Delaunay properties
    */
    struct Delaunay
    {
        //! Delaunay constructor.
        /*!
          Constructor given a list of 2D points triangulates based on Delaunay properties
        */
        Delaunay(const std::vector<glm::vec2>& _points);
        //! Delaunay constructor.
        /*!
          Constructor given a list of 2D points triangulates based on Delaunay properties.
          Then constrains the triangles generated to the given vector of linked constrains
        */
        Delaunay(const std::vector<glm::vec2>& _points, std::vector<Edge>& _constraints);

        //! A getting to calculate ordered points
        /*!
          Given a vector of edges, calculates unique points from edges and already generated triangles.
          Returns a vector of vectors that each contains continuous ordered points that are connected
        */
        std::vector<std::vector<glm::vec2> > getOrderedTrianglePoints(std::vector<Edge>& _outerEdges);

        //! Public variable.
        /*!
          Vector of generated triangles from delaunay triangulation
        */
        std::vector<Triangle> m_triangles;
    private:
        //! Super triangle.
        /*!
          generated triangle that encloses all given points, used in triangulation
        */
        Triangle m_superTriangle;
        void generateSuperTriangle(const std::vector<glm::vec2>& _points);
        void BowyerWatson(const std::vector<glm::vec2>& _points);

        void removeTrisInHole(const std::vector<glm::vec2>& _holePoints, const std::vector<glm::vec2>& _meshPoints);
        bool checkPointWithinPolygon(const glm::vec2& _point, const std::vector<glm::vec2>& _polygon);

        //! Returns vector of vectors of unique points.
        /*!
          given a vector of edges returns vector of vectors that have unique linked points
        */
        std::vector<std::vector<glm::vec2> > getEdgeUniquePoints(std::vector<Edge>& _constraints);
        //! Returns vector of unique points.
        /*!
          given a vector of points returns vector that has unique points
        */
        std::vector<glm::vec2> getUniquePoints(const std::vector<glm::vec2>& _points);
    };

    //! Custom comparator for glm::vec2.
    /*!
      used as an external comparator for glm::vec2
    */
    struct Vec2Comparator
    {
        bool operator()(const glm::vec2& a, const glm::vec2& b) const {
            if (a.x != b.x) return a.x < b.x;
            return a.y < b.y;
        }
    };
}

