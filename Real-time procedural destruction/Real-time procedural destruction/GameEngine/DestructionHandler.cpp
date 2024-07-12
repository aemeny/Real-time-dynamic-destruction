#include "DestructionHandler.h"
#include "../Physics/TraceRay.h"

#include <random>

namespace GameEngine
{
    DestructionHandler::~DestructionHandler() {}

    std::vector<glm::vec3> DestructionHandler::destructObject(intersectionInfo* _info, std::weak_ptr<Transform> _transform)
    {
        // Grab faces of model
        //std::vector<bu::Face>* faces = _info->intersectedModel.lock()->getFaces();

        // Use normal vector of the collided face to determine projection
        ProjectionPlane plane = determineProjectionPlane(_info->collidedFace);

        // Generates a set number of points randomly in a 2D square aligned to the faces normal
        std::vector<glm::vec3> generatedPoints = generateSquarePoints(_info->intersectionPos, 3.5f, 20, plane, _transform); //Intersect pos, Square diameter, Depth (num points), plane to project to, model transform
       
        // Create a delaunay diagram from the new generated points, after projecting them into 2D
        Delaunay delaunayDiagram = Delaunay(projectVertices(&generatedPoints, plane));



        return generatedPoints;
        // Update model
        //core().lock()->m_traceRay->getObjectsInScene()->at(_info->objIndex)->transform().lock()->setDirty(true);
        //_info->intersectedModel.lock()->updateModel();
    }


    ProjectionPlane DestructionHandler::determineProjectionPlane(const bu::Face* _collidedFace)
    {
        //Find absoulate value of each normal
        float absX = std::abs(_collidedFace->na.x);
        float absY = std::abs(_collidedFace->na.y);
        float absZ = std::abs(_collidedFace->na.z);

        // Find which normal is largest
        if (absX > absY && absX > absZ) 
        {
            return ProjectionPlane::YZ;  // Normal is aligned to the x axis, project onto YZ plane
        }
        else if (absY > absZ) 
        {
            return ProjectionPlane::XZ;  // Normal is aligned to the y axis, project onto XZ plane
        }
        else 
        {
            return ProjectionPlane::XY;  // Normal is aligned to the z axis, project onto XY plane
        }
    }

    std::vector<glm::vec3> DestructionHandler::generateSquarePoints(glm::vec3 _pos, float _size, int _depth, ProjectionPlane _plane, std::weak_ptr<Transform> _transform)
    {
        std::vector<glm::vec3> points;
        float halfSize = _size * 0.5f;
        glm::vec3 modelPos = _transform.lock()->getPos();
        glm::vec2 modelDiameter;

        /*
           Looks to make sure there are reasonanal amount of points to destruct before continuing
           After depth is ended, try to aim for atleast 6 points in given area, to decrease loops / computation time minimum needed decreases every loop
        */
        int minimumThreashold = _depth + 6;

        int i = 0;
        switch (_plane) // Switchs which plane I need to keep impact position
        {
        case XY: // Cuts the z axis
            modelDiameter = glm::vec2(_transform.lock()->getScale().x, _transform.lock()->getScale().y);

            while (i < _depth || points.size() < minimumThreashold)
            {
                float rand1 = ((float)rand() / (RAND_MAX)) * 2 - 1;
                float rand2 = ((float)rand() / (RAND_MAX)) * 2 - 1;
                float point1 = _pos.x + ((rand1 * halfSize));
                float point2 = _pos.y + ((rand2 * halfSize));

                if (glm::abs(point1 - modelPos.x) < modelDiameter.x && glm::abs(point2 - modelPos.y) < modelDiameter.y) // If in bounds of model
                {
                    points.push_back(glm::vec3(point1, point2, _pos.z));
                }

                if (minimumThreashold > 1) // Makes sure atleast one point is created in area before continuing
                    minimumThreashold--; //Stops loop taking too much time if not found quick enough
                i++;
                if (i > 100)
                {
                    std::cout << "Too many checks, broke out point creation! \n";
                    break;
                }
            }

            break;
        case YZ: // Cuts the x axis
            modelDiameter = glm::vec2(_transform.lock()->getScale().y, _transform.lock()->getScale().z);

            while (i < _depth || points.size() < minimumThreashold)
            {
                float rand1 = ((float)rand() / (RAND_MAX)) * 2 - 1;
                float rand2 = ((float)rand() / (RAND_MAX)) * 2 - 1;
                float point1 = _pos.y + ((rand1 * halfSize));
                float point2 = _pos.z + ((rand2 * halfSize));

                if (glm::abs(point1 - modelPos.y) < modelDiameter.x && glm::abs(point2 - modelPos.z) < modelDiameter.y) // If in bounds of model
                {
                    points.push_back(glm::vec3(_pos.x, point1, point2));
                }

                if (minimumThreashold > 1)
                    minimumThreashold--;
                i++;
                if (i > 100)
                {
                    std::cout << "Too many checks, broke out point creation! \n";
                    break;
                }
            }
            break;
        case XZ: // Cuts the y axis
            modelDiameter = glm::vec2(_transform.lock()->getScale().x, _transform.lock()->getScale().z);

            while (i < _depth || points.size() < minimumThreashold)
            {
                float rand1 = ((float)rand() / (RAND_MAX)) * 2 - 1;
                float rand2 = ((float)rand() / (RAND_MAX)) * 2 - 1;
                float point1 = _pos.x + ((rand1 * halfSize));
                float point2 = _pos.z + ((rand2 * halfSize));

                if (glm::abs(point1 - modelPos.x) < modelDiameter.x && glm::abs(point2 - modelPos.z) < modelDiameter.y) // If in bounds of model
                {
                    points.push_back(glm::vec3(point1, _pos.y, point2));
                }

                if (minimumThreashold > 1)
                    minimumThreashold--;
                i++;
                if (i > 100)
                {
                    std::cout << "Too many checks, broke out point creation! \n";
                    break;
                }
            }
            break;
        }

        return points;
    }

    std::vector<glm::vec2> DestructionHandler::projectVertices(const std::vector<glm::vec3>* _points, ProjectionPlane _plane)
    {
        std::vector<glm::vec2> projectedVertices;
        projectedVertices.reserve(_points->size());
        switch (_plane) // Switchs which plane I need to points from 3D vertices to 2D
        {
        case XY: // Cuts the z axis
            for (const glm::vec3& point : *_points)
            {
                projectedVertices.emplace_back(point.x, point.y);
            }
        break;
        case YZ: // Cuts the x axis
            for (const glm::vec3& point : *_points)
            {
                projectedVertices.emplace_back(point.y, point.z);
            }
        break;
        case XZ: // Cuts the y axis
            for (const glm::vec3& point : *_points)
            {
                projectedVertices.emplace_back(point.x, point.z);
            }
        break;
        }
        return projectedVertices;
    }
}