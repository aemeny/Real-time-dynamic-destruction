#include "DestructionHandler.h"
#include "../Physics/TraceRay.h"

#include <random>
#include <map>

namespace GameEngine
{
    DestructionHandler::~DestructionHandler() {}

    std::vector<Triangle> DestructionHandler::destructObject(intersectionInfo* _info, std::weak_ptr<Transform> _transform)
    {
        // Use normal vector of the collided face to determine projection
        ProjectionPlane plane = determineProjectionPlane(_info->collidedFace);

        // Generates a set number of points randomly in a 2D square aligned to the faces normal
        std::vector<glm::vec3> generatedPoints = generateSquarePoints(_info->intersectionPos, 3.0f, 30, plane, _transform); //Intersect pos, Square diameter, Depth (num points), plane to project to, model transform
       
        // Create a delaunay diagram from the new generated points, after projecting them into 2D
        float savedUnprojetedPoint;
        Delaunay delaunayDiagram = Delaunay(projectVertices(&generatedPoints, plane, savedUnprojetedPoint));

        // Create Voronoi diagram from generate delaunay triangles
        VoronoiDiagram voronoiDiagram;
        voronoiDiagram.generate(delaunayDiagram.m_triangles);

        // Cut edges to bounds of model that go out of area
        LineClippingAlgorithm lineClipper = LineClippingAlgorithm(_transform, plane);
        for (VoronoiCell& cell : voronoiDiagram.m_voronoiCells) 
        {
            std::vector<Edge> cutEdges;
            std::vector<Edge> connectingEdges;
            for (Edge& edge : cell.m_edges)
            {
                Edge clippedEdge = lineClipper.CohenSutherland(edge);
                if (clippedEdge.m_start.x != -1) // Only keep edges that are valid and in range
                {
                    cutEdges.push_back(clippedEdge);
                    if (clippedEdge.m_clipped != 0) // Has been clipped. 1 == start, 2 == end vertex
                        connectingEdges.push_back(clippedEdge);
                }
                if (connectingEdges.size() == 2) // If two edges have been cliped, connect their cut vertices
                {
                    glm::vec2 clippedVertex1 = connectingEdges[0].m_clipped == 1 ? connectingEdges[0].m_start : connectingEdges[0].m_end;
                    glm::vec2 clippedVertex2 = connectingEdges[1].m_clipped == 1 ? connectingEdges[1].m_start : connectingEdges[1].m_end;
                    cutEdges.emplace_back(Edge{ clippedVertex1, clippedVertex2 });
                }
            }
            cell.m_edges.swap(cutEdges); // Replace old edges with new clipped edges vector
            if (cell.m_edges.size() < 3) // Not enough edges to create a cell
            {
                cell.m_edges.clear();
            }
        }

        // Generate a convex hull cell for the orignal squares cutout
        std::vector<VoronoiCell> convexHull;
        convexHull.emplace_back(voronoiDiagram.generateConvexHull(voronoiDiagram.m_voronoiCells));

        // Grab faces of model with convex hull point to retriangulate
        Delaunay newModelTriangles = Delaunay(getNewMeshVertices(_info->intersectedModel.lock()->getFaces(), convexHull[0], _transform, plane));

        return newModelTriangles.m_triangles;

        // Unproject vertices back into 3D space, 
        //return unProjectVertices(&convexHull, plane, savedUnprojetedPoint);
        //return unProjectVertices(&voronoiDiagram.m_voronoiCells, plane, savedUnprojetedPoint);

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
        float halfSize;
        glm::vec3 modelPos = _transform.lock()->getPos();
        glm::vec2 modelDiameter;

        /*
           Looks to make sure there are reasonanal amount of points to destruct before continuing
           After depth is ended, try to aim for atleast 6 points in given area, to decrease loops / computation time minimum needed decreases every loop
        */
        int minimumThreashold = _depth + 10;

        int i = 0;
        switch (_plane) // Switchs which plane I need to keep impact position
        {
        case XY: // Cuts the z axis
            modelDiameter = glm::vec2(_transform.lock()->getScale().x, _transform.lock()->getScale().y);

            _size = _size < modelDiameter.x && _size < modelDiameter.y ? _size : _size > modelDiameter.x ? modelDiameter.x : modelDiameter.y;
            halfSize = _size * 0.5f;

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

                if (minimumThreashold > 6) // Makes sure atleast one point is created in area before continuing
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

            _size = _size < modelDiameter.x && _size < modelDiameter.y ? _size : _size > modelDiameter.x ? modelDiameter.x : modelDiameter.y;
            halfSize = _size * 0.5f;

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

                if (minimumThreashold > 6)
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

            _size = _size < modelDiameter.x && _size < modelDiameter.y ? _size : _size > modelDiameter.x ? modelDiameter.x : modelDiameter.y;
            halfSize = _size * 0.5f;

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

                if (minimumThreashold > 6)
                    minimumThreashold--;
                i++;
                if (i > 100)
                {
                    std::cout << "Too many checks, broke out point creation! \n";
                    points.clear(); // Clear any points that might have been made
                    break;
                }
            }
            break;
        }
        points.push_back(glm::vec3(_pos)); // Always have a point on impact

        return points;
    }

    std::vector<glm::vec2> DestructionHandler::getNewMeshVertices(const std::vector<bu::Face>* _faces, VoronoiCell& _convexhull, const std::weak_ptr<Transform>& _transform, ProjectionPlane _plane)
    {
        std::vector<glm::vec2> uniquePoints;
        glm::vec3 pos = _transform.lock()->getPos();
        glm::vec3 scale = _transform.lock()->getScale();

        switch (_plane) // Switchs which plane I need to points from 3D vertices to 2D
        {
        case XY: // Cuts the z axis
            for (const bu::Face& face : *_faces)
            {
                glm::vec2 point1 = { (face.pa.x * scale.x) + pos.x, (face.pa.y * scale.y) + pos.y };
                glm::vec2 point2 = { (face.pb.x * scale.x) + pos.x, (face.pb.y * scale.y) + pos.y };
                glm::vec2 point3 = { (face.pc.x * scale.x) + pos.x, (face.pc.y * scale.y) + pos.y };
                _convexhull.m_edges.emplace_back(point1, point2);
                _convexhull.m_edges.emplace_back(point2, point3);
                _convexhull.m_edges.emplace_back(point3, point1);
            }
            break;
        case YZ: // Cuts the x axis
            for (const bu::Face& face : *_faces)
            {
                glm::vec2 point1 = { (face.pa.z * scale.z) + pos.z, (face.pa.y * scale.y) + pos.y };
                glm::vec2 point2 = { (face.pb.z * scale.z) + pos.z, (face.pb.y * scale.y) + pos.y };
                glm::vec2 point3 = { (face.pc.z * scale.z) + pos.z, (face.pc.y * scale.y) + pos.y };
                _convexhull.m_edges.emplace_back(point1, point2);
                _convexhull.m_edges.emplace_back(point2, point3);
                _convexhull.m_edges.emplace_back(point3, point1);
            }
            break;
        case XZ: // Cuts the y axis
            for (const bu::Face& face : *_faces)
            {
                glm::vec2 point1 = { (face.pa.x * scale.x), (face.pa.z * scale.z) + pos.z };
                glm::vec2 point2 = { (face.pb.x * scale.x), (face.pb.z * scale.z) + pos.z };
                glm::vec2 point3 = { (face.pc.x * scale.x), (face.pc.z * scale.z) + pos.z };
                _convexhull.m_edges.emplace_back(point1, point2);
                _convexhull.m_edges.emplace_back(point2, point3);
                _convexhull.m_edges.emplace_back(point3, point1);
            }
            break;
        }

        // Remove duplicate edges
        std::sort(_convexhull.m_edges.begin(), _convexhull.m_edges.end());
        _convexhull.m_edges.erase(std::unique(_convexhull.m_edges.begin(), _convexhull.m_edges.end(),
            [](const Edge& a, const Edge& b) {
                return (a == b);
            }), _convexhull.m_edges.end());

        // Add all convex hull vertices
        for (const Edge& edge : _convexhull.m_edges)
        {
            uniquePoints.push_back(edge.m_start);
            uniquePoints.push_back(edge.m_end);
        }

        return uniquePoints;
    }

    std::vector<glm::vec2> DestructionHandler::projectVertices(const std::vector<glm::vec3>* _points, ProjectionPlane _plane, float& _savedPoint)
    {
        std::vector<glm::vec2> projectedVertices;
        projectedVertices.reserve(_points->size());
        switch (_plane) // Switchs which plane I need to points from 3D vertices to 2D
        {
        case XY: // Cuts the z axis
            _savedPoint = _points[0][0].z;
            for (const glm::vec3& point : *_points)
            {
                projectedVertices.emplace_back(point.x, point.y);
            }
        break;
        case YZ: // Cuts the x axis
            _savedPoint = _points[0][0].x;
            for (const glm::vec3& point : *_points)
            {
                projectedVertices.emplace_back(point.z, point.y);
            }
        break;
        case XZ: // Cuts the y axis
            _savedPoint = _points[0][0].y;
            for (const glm::vec3& point : *_points)
            {
                projectedVertices.emplace_back(point.x, point.z);
            }
        break;
        }
        return projectedVertices;
    }

    std::vector<VoronoiCell3D> DestructionHandler::unProjectVertices(const std::vector<VoronoiCell>* _cells, ProjectionPlane _plane, float _savedPoint)
    {
        std::vector<VoronoiCell3D> projectedCells;
        projectedCells.reserve(_cells->size());
        switch (_plane) // Switchs which plane I need to points from 3D vertices to 2D
        {
        case XY: // Adds the z axis
            for (const VoronoiCell& cell : *_cells)
            {
                std::vector<Edge3D> edges3D;
                for (const Edge& edge : cell.m_edges)
                {
                    edges3D.emplace_back(Edge3D(glm::vec3(edge.m_start, _savedPoint), glm::vec3(edge.m_end, _savedPoint)));
                }
                projectedCells.emplace_back(VoronoiCell3D{ edges3D });
            }
            break;
        case YZ: // Adds the x axis
            for (const VoronoiCell& cell : *_cells)
            {
                std::vector<Edge3D> edges3D;
                for (const Edge& edge : cell.m_edges)
                {
                    edges3D.emplace_back(Edge3D(glm::vec3(_savedPoint, edge.m_start.y, edge.m_start.x), glm::vec3(_savedPoint, edge.m_start.y, edge.m_start.x)));
                }
                projectedCells.emplace_back(VoronoiCell3D{ edges3D });
            }
            break;
        case XZ: // Adds the y axis
            for (const VoronoiCell& cell : *_cells)
            {
                std::vector<Edge3D> edges3D;
                for (const Edge& edge : cell.m_edges)
                {
                    edges3D.emplace_back(Edge3D(glm::vec3(edge.m_start.x, _savedPoint, edge.m_start.y), glm::vec3(edge.m_end.x, _savedPoint, edge.m_start.y)));
                }
                projectedCells.emplace_back(VoronoiCell3D{ edges3D });
            }
            break;
        }

        return projectedCells;
    }
}
