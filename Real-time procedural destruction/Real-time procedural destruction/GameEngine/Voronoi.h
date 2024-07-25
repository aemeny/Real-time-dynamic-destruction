#pragma once
#include "Delaunay.h"

namespace GameEngine
{
    //!  Voronoi Cell Struct. 
    /*!
      Holds information about each create voronoi cell including, a vector of the edges that make up the cell
      and a site which is the centre of the cell
    */
    struct VoronoiCell
    {
        std::vector<Edge> m_edges;
        glm::vec2 m_site;
    };

    //!  Voronoi Diagram Struct. 
    /*!
      Generates and contains the whole voronoi diagram when passed the generated delaunay triangles
    */
    struct VoronoiDiagram
    {
        //! Voronoi Diagram constructor.
        /*!
          The default constructor used to just create the object
        */
        VoronoiDiagram();
        
        //! Generate diagram.
        /*!
          The main function that generates the voronoi diagram based on the given delaunay triangulation
        */
        void generate(const std::vector<Triangle>& _delaunayTriangles);
        //! Convex hull generation.
        /*!
          Generate a voronoi cell that is the convex hull (outer edges) of the voronoi diagram
        */
        VoronoiCell generateConvexHull(const std::vector<VoronoiCell>& _cells);

        // Vectors of all the voronoi cells in the diagram
        std::vector<VoronoiCell> m_voronoiCells;
    };
}

