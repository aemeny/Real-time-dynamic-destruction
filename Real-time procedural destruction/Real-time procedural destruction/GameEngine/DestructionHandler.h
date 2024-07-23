#pragma once
#include "LineClippingAlgorithm.h"
#include "../Physics/BaseCollider.h"

namespace GameEngine
{
    //!  DestructionHandler class. 
    /*!
      Components that cna be added to handle everything needed to generate destruction based on voronoi diagrams
    */
    struct DestructionHandler : Component
    {
        ~DestructionHandler();
        //! Core constructor.
        /*!
          The default constructor used to just create object and set defualt values to be changed later
        */
        DestructionHandler();

        //! Main function for destruction.
        /*!
          Takes in all the information about the intersection and the object itself and runs
          through a process of using voronoi and delaunay triangulation to create a hole
          and update the model
        */
        void destructObject(intersectionInfo* _info, std::weak_ptr<Transform> _transform);

        //! Setter.
        /*!
          Sets a bool if the random point generation should be from inside a circle or a square
        */
        void setRandomFromCircle(bool _fromCircle) { m_randomFromCircle = _fromCircle; }
        //! Setter.
        /*!
          Sets a value for the diameter of the point generation for destruction, value caps at objects scale
        */
        void setDestructionDiameter(float _diameter) { m_destructionDiameter = _diameter; }
        //! Setter.
        /*!
          Sets a value for the amount of points the random function should try to generate
        */
        void setPointGenerationDepth(int _depth) { m_pointGenerateDepth = _depth; }
    private:
        // Private variables holds information about how the destruction should be customised
        bool m_randomFromCircle;
        float m_destructionDiameter;
        int m_pointGenerateDepth;

        //! Projection based functions.
        /*!
          - Determines projection plane direction based on the collided faces normals
          - Projects the 3D vertices into 2D by cutting the determined flat plane
          - Unprojects the the given 2D vertices back into 3D from the saved cut point
        */
        ProjectionPlane determineProjectionPlane(const bu::Face* _collidedFace);
        std::vector<glm::vec2> projectVertices(const std::vector<glm::vec3>* _points, ProjectionPlane _plane, float& _savedPoint);
        glm::vec3 unProjectVertex(const glm::vec2& _point, ProjectionPlane _plane, float _savedPoint);
        
        //! Random generation based functions.
        /*!
          - Generates random given number of points over an area within given / found bounds
          - Generates a random point in a square
          - Generates a random point in a circle
        */
        std::vector<glm::vec3> generateRandomPoints(glm::vec3 _pos, ProjectionPlane _plane, std::weak_ptr<Transform> _transform);
        glm::vec2 randomPointInSquare(glm::vec2 _pos);
        glm::vec2 randomPointInCircle(glm::vec2 _pos);

        //! Getter for the collided models vertices
        /*!
          Finds the world space vertices on the determined plane from the given model
        */
        std::vector<glm::vec2> modelVertices(const std::vector<bu::Face>* _faces, const std::weak_ptr<Transform>& _transform, ProjectionPlane _plane);
        //! Cuts lines that are out of bounds
        /*!
          Loops through each voronoi cell edge and clips the lines to the bounds of the model
        */
        void cutEdges(std::vector<VoronoiCell>* _voronoiCells, LineClippingAlgorithm* _lineClipper);
    
        //! Adds the new created faces
        /*!
          Generates a vector of bu::Face's based on the new tris generated to be updated in the model later.
          Takes into account the projected plane and places them back into 3D using the unProjectVertex() func
        */
        void addNewFaces(std::vector<bu::Face>* _newFaces, const std::vector<Triangle>& _tris, const std::weak_ptr<Transform>& _transform, ProjectionPlane _plane, float _savedPoint);
        //! Connects the back and front face
        /*!
          Given the inner holes edge vertices connects the back and front face with new bu::Face's to fill the holes new inner walls.
          Provided the edges are in a connected winding order.
        */
        void connectFaces(std::vector<bu::Face>* _newFaces, std::vector<std::vector<glm::vec2> >& _edgePoints, const std::weak_ptr<Transform>& _transform, ProjectionPlane _plane, float _savedPoint);
    };
}

