#pragma once
#include "Component.h"
#include "LineClippingAlgorithm.h"
#include "../Physics/BaseCollider.h"

namespace GameEngine
{
    struct DestructionHandler : Component
    {
        ~DestructionHandler();
        DestructionHandler();

        void destructObject(intersectionInfo* _info, std::weak_ptr<Transform> _transform);

        void setRandomFromCircle(bool _fromCircle) { m_randomFromCircle = _fromCircle; }
        void setDestructionDiameter(float _diameter) { m_destructionDiameter = _diameter; }
        void setPointGenerationDepth(int _depth) { m_pointGenerateDepth = _depth; }
    private:
        bool m_randomFromCircle;
        float m_destructionDiameter;
        int m_pointGenerateDepth;

        ProjectionPlane determineProjectionPlane(const bu::Face* _collidedFace);
        std::vector<glm::vec2> projectVertices(const std::vector<glm::vec3>* _points, ProjectionPlane _plane, float& _savedPoint);
        glm::vec3 unProjectVertex(const glm::vec2& _point, ProjectionPlane _plane, float _savedPoint);
        
        std::vector<glm::vec3> generateRandomPoints(glm::vec3 _pos, ProjectionPlane _plane, std::weak_ptr<Transform> _transform);
        glm::vec2 randomPointInSquare(glm::vec2 _pos);
        glm::vec2 randomPointInCircle(glm::vec2 _pos);

        std::vector<glm::vec2> modelVertices(const std::vector<bu::Face>* _faces, const std::weak_ptr<Transform>& _transform, ProjectionPlane _plane);
        void cutEdges(std::vector<VoronoiCell>* _voronoiCells, LineClippingAlgorithm* _lineClipper);
    
        void addNewFaces(std::vector<bu::Face>* _newFaces, const std::vector<Triangle>& _tris, const std::weak_ptr<Transform>& _transform, ProjectionPlane _plane, float _savedPoint);
        void connectFaces(std::vector<bu::Face>* _newFaces, std::vector<std::vector<glm::vec2> >& _edgePoints, const std::weak_ptr<Transform>& _transform, ProjectionPlane _plane, float _savedPoint);
    };
}

