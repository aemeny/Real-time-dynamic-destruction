#include "DestructionHandler.h"
#include "../Physics/TraceRay.h"

#include <random>
#include <map>

namespace GameEngine
{
    DestructionHandler::~DestructionHandler() {}

    void DestructionHandler::destructObject(intersectionInfo* _info, std::weak_ptr<Transform> _transform)
    {
        // Use normal vector of the collided face to determine projection
        ProjectionPlane plane = determineProjectionPlane(_info->collidedFace);

        // Generates a set number of points randomly in a 2D square aligned to the faces normal
        std::vector<glm::vec3> generatedPoints = generateCirclePoints(_info->intersectionPos, 4.0f, 8, plane, _transform); //Intersect pos, Square diameter, Depth (num points), plane to project to, model transform

        // Create a delaunay diagram from the new generated points, after projecting them into 2D
        float savedUnprojetedPoint;
        Delaunay delaunayDiagram = Delaunay(projectVertices(&generatedPoints, plane, savedUnprojetedPoint));

        // Create Voronoi diagram from generate delaunay triangles
        VoronoiDiagram voronoiDiagram;
        voronoiDiagram.generate(delaunayDiagram.m_triangles);

        // Cut edges to bounds of model that go out of area
        LineClippingAlgorithm lineClipper = LineClippingAlgorithm(_transform, plane);

        std::vector<Edge> connectingEdges;
        for (VoronoiCell& cell : voronoiDiagram.m_voronoiCells)
        {
            std::vector<Edge> cutEdges;
            for (Edge& edge : cell.m_edges)
            {
                Edge clippedEdge = lineClipper.CohenSutherland(edge);
                if (clippedEdge.m_start.x != -1) // Only keep edges that are valid and in range
                {
                    cutEdges.push_back(clippedEdge);
                    if (clippedEdge.m_clipped != 0) // Has been clipped. 1 == start, 2 == end vertex
                    {
                        connectingEdges.push_back(clippedEdge);
                    }
                }
            }
            if (connectingEdges.size() == 2) // If two edges have been clipped, connect their cut vertices
            {
                glm::vec2 clippedVertex1 = connectingEdges[0].m_clipped == 1 ? connectingEdges[0].m_start : connectingEdges[0].m_end;
                glm::vec2 clippedVertex2 = connectingEdges[1].m_clipped == 1 ? connectingEdges[1].m_start : connectingEdges[1].m_end;
                cutEdges.emplace_back(Edge{ clippedVertex1, clippedVertex2 });
                connectingEdges.clear();
            }

            cell.m_edges.swap(cutEdges); // Replace old edges with new clipped edges vector
            if (cell.m_edges.size() < 3) // Not enough edges to create a cell
            {
                cell.m_edges.clear();
            }
        }

        // Generate a convex hull cell for the orignal squares cutout
        VoronoiCell convexHull = voronoiDiagram.generateConvexHull(voronoiDiagram.m_voronoiCells);

        // Grab models faces
        std::vector<bu::Face>* faces = _info->intersectedModel.lock()->getFaces();

        // Grab faces of model with convex hull point to retriangulate
        Delaunay newModelTriangles = Delaunay(modelVertices(faces, _transform, plane), convexHull.m_edges);

        // Remove old faces and add the new ones
        std::vector<bu::Face> newFaces;
        glm::vec3 pos = _transform.lock()->getPos();
        glm::vec3 scale = _transform.lock()->getScale();

        /*switch (plane)
        {
        case XY:
            for (const bu::Face& face : *faces)
            {
                if (face.na.z != 1 && face.na.z != -1)
                {
                    newFaces.push_back(face);
                }
            }
            break;
        case YZ:
            for (const bu::Face& face : *faces)
            {
                if (face.na.x != 1 && face.na.x != -1)
                {
                    newFaces.push_back(face);
                }
            }
            break;
        case XZ:
            for (const bu::Face& face : *faces)
            {
                if (face.na.y != 1 && face.na.y != -1)
                {
                    newFaces.push_back(face);
                }
            }
            break;
        }*/
        for (const Triangle& tri : newModelTriangles.m_triangles)
        {
            bu::Face frontNewFace;
            // Assign vertex positions
            frontNewFace.pa = unProjectVertex({ (tri.m_vertices[0].x - pos.x) / scale.x, (tri.m_vertices[0].y - pos.y) / scale.y }, plane, (savedUnprojetedPoint - pos.z) / scale.z);
            frontNewFace.pb = unProjectVertex({ (tri.m_vertices[1].x - pos.x) / scale.x, (tri.m_vertices[1].y - pos.y) / scale.y }, plane, (savedUnprojetedPoint - pos.z) / scale.z);
            frontNewFace.pc = unProjectVertex({ (tri.m_vertices[2].x - pos.x) / scale.x, (tri.m_vertices[2].y - pos.y) / scale.y }, plane, (savedUnprojetedPoint - pos.z) / scale.z);

            // Assuming you also have texture coordinates and normals calculated
            frontNewFace.tca = glm::vec2(frontNewFace.pa.x * scale.x * 0.25f, frontNewFace.pa.y * scale.y * 0.25f);
            frontNewFace.tcb = glm::vec2(frontNewFace.pb.x * scale.x * 0.25f, frontNewFace.pb.y * scale.y * 0.25f);
            frontNewFace.tcc = glm::vec2(frontNewFace.pc.x * scale.x * 0.25f, frontNewFace.pc.y * scale.y * 0.25f);

            frontNewFace.na = glm::vec3(0, 0, 1);
            frontNewFace.nb = glm::vec3(0, 0, 1);
            frontNewFace.nc = glm::vec3(0, 0, 1);

            frontNewFace.lmca = glm::vec2(0); // Default or calculated value
            frontNewFace.lmcb = glm::vec2(0);
            frontNewFace.lmcc = glm::vec2(0);

            // Add the new face to the list of faces
            newFaces.push_back(frontNewFace);

            bu::Face backNewFace;
            // Assign vertex positions
            backNewFace.pa = unProjectVertex({ (tri.m_vertices[0].x - pos.x) / scale.x, (tri.m_vertices[0].y - pos.y) / scale.y }, plane, ((savedUnprojetedPoint - scale.z * 2.0f) - pos.z) / scale.z);
            backNewFace.pb = unProjectVertex({ (tri.m_vertices[1].x - pos.x) / scale.x, (tri.m_vertices[1].y  - pos.y) / scale.y }, plane, ((savedUnprojetedPoint - scale.z * 2.0f) - pos.z) / scale.z);
            backNewFace.pc = unProjectVertex({ (tri.m_vertices[2].x - pos.x) / scale.x, (tri.m_vertices[2].y - pos.y) / scale.y }, plane, ((savedUnprojetedPoint - scale.z * 2.0f) - pos.z) / scale.z);

            // Assuming you also have texture coordinates and normals calculated
            backNewFace.tca = glm::vec2(backNewFace.pa.x * scale.x * 0.25f, backNewFace.pa.y * scale.y * 0.25f);
            backNewFace.tcb = glm::vec2(backNewFace.pb.x * scale.x * 0.25f, backNewFace.pb.y * scale.y * 0.25f);
            backNewFace.tcc = glm::vec2(backNewFace.pc.x * scale.x * 0.25f, backNewFace.pc.y * scale.y * 0.25f);

            backNewFace.na = glm::vec3(0, 0, -1);
            backNewFace.nb = glm::vec3(0, 0, -1);
            backNewFace.nc = glm::vec3(0, 0, -1);

            backNewFace.lmca = glm::vec2(0); // Default or calculated value
            backNewFace.lmcb = glm::vec2(0);
            backNewFace.lmcc = glm::vec2(0);

            // Add the new face to the list of faces
            newFaces.push_back(backNewFace);
        }

        std::vector<Edge> outerEdges;
        for (const bu::Face& face : *faces)
        {
            outerEdges.push_back(Edge(face.pa, face.pb));
            outerEdges.push_back(Edge(face.pb, face.pc));
            outerEdges.push_back(Edge(face.pc, face.pa));
        }
        // Connect the front face to the back face by filling the hole in the mesh with new triangles
        std::vector<std::vector<glm::vec2> > orderedEdgePoints = newModelTriangles.getOrderedTrianglePoints(outerEdges);
        for (int i = 0; i < orderedEdgePoints.size(); i++)
        {
            for (int j = 0; j < orderedEdgePoints[i].size(); j++)
            {
                int nextPoint;
                if (j == orderedEdgePoints[i].size() - 1)
                    nextPoint = 0;
                else
                    nextPoint = j + 1;

                bu::Face firstNewFace;
                // Assign vertex positions
                firstNewFace.pa = unProjectVertex({ (orderedEdgePoints[i][j].x - pos.x) / scale.x, (orderedEdgePoints[i][j].y - pos.y) / scale.y}, plane, (savedUnprojetedPoint - pos.z) / scale.z);
                firstNewFace.pb = unProjectVertex({ (orderedEdgePoints[i][j].x - pos.x) / scale.x, (orderedEdgePoints[i][j].y - pos.y) / scale.y }, plane, ((savedUnprojetedPoint - scale.z * 2.0f) - pos.z) / scale.z);
                firstNewFace.pc = unProjectVertex({ (orderedEdgePoints[i][nextPoint].x - pos.x) / scale.x, (orderedEdgePoints[i][nextPoint].y - pos.y) / scale.y }, plane, ((savedUnprojetedPoint - scale.z * 2.0f) - pos.z) / scale.z);

                // Assuming you also have texture coordinates and normals calculated
                firstNewFace.tca = glm::vec2(firstNewFace.pa.x * scale.x * 0.25f, firstNewFace.pa.z * scale.z * 0.25f);
                firstNewFace.tcb = glm::vec2(firstNewFace.pb.x * scale.x * 0.25f, firstNewFace.pb.z * scale.z * 0.25f);
                firstNewFace.tcc = glm::vec2(firstNewFace.pc.x * scale.x * 0.25f, firstNewFace.pc.z * scale.z * 0.25f);

                firstNewFace.na = glm::vec3(0, 1, 0);
                firstNewFace.nb = glm::vec3(0, 1, 0);
                firstNewFace.nc = glm::vec3(0, 1, 0);

                firstNewFace.lmca = glm::vec2(0); // Default or calculated value
                firstNewFace.lmcb = glm::vec2(0);
                firstNewFace.lmcc = glm::vec2(0);

                // Add the new face to the list of faces
                newFaces.push_back(firstNewFace);

                bu::Face secondNewFace;
                // Assign vertex positions
                secondNewFace.pa = unProjectVertex({ (orderedEdgePoints[i][j].x - pos.x) / scale.x, (orderedEdgePoints[i][j].y - pos.y) / scale.y }, plane, (savedUnprojetedPoint - pos.z) / scale.z);
                secondNewFace.pb = unProjectVertex({ (orderedEdgePoints[i][nextPoint].x - pos.x) / scale.x, (orderedEdgePoints[i][nextPoint].y - pos.y) / scale.y }, plane, (savedUnprojetedPoint - pos.z) / scale.z);
                secondNewFace.pc = unProjectVertex({ (orderedEdgePoints[i][nextPoint].x - pos.x) / scale.x, (orderedEdgePoints[i][nextPoint].y - pos.y) / scale.y }, plane, ((savedUnprojetedPoint - scale.z * 2.0f) - pos.z) / scale.z);

                // Assuming you also have texture coordinates and normals calculated
                secondNewFace.tca = glm::vec2(secondNewFace.pa.x * scale.x * 0.25f, secondNewFace.pa.z * scale.z * 0.25f);
                secondNewFace.tcb = glm::vec2(secondNewFace.pb.x * scale.x * 0.25f, secondNewFace.pb.z * scale.z * 0.25f);
                secondNewFace.tcc = glm::vec2(secondNewFace.pc.x * scale.x * 0.25f, secondNewFace.pc.z * scale.z * 0.25f);

                secondNewFace.na = glm::vec3(0, 1, 0);
                secondNewFace.nb = glm::vec3(0, 1, 0);
                secondNewFace.nc = glm::vec3(0, 1, 0);

                secondNewFace.lmca = glm::vec2(0); // Default or calculated value
                secondNewFace.lmcb = glm::vec2(0);
                secondNewFace.lmcc = glm::vec2(0);

                // Add the new face to the list of faces
                newFaces.push_back(secondNewFace);
            }
        }

        //Update Model
        _info->intersectedModel.lock()->setVertices(newFaces.size() * 3);
        _info->intersectedModel.lock()->setFaces(newFaces);
        _info->intersectedModel.lock()->updateModel();
        _info->intersectedModel.lock()->setDestruction(false);
        _transform.lock()->setDirty(true);
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

        srand(time(NULL));
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

    std::vector<glm::vec3> DestructionHandler::generateCirclePoints(glm::vec3 _pos, float _size, int _depth, ProjectionPlane _plane, std::weak_ptr<Transform> _transform)
    {
        std::vector<glm::vec3> points;
        float halfSize;
        glm::vec3 modelPos = _transform.lock()->getPos();
        glm::vec2 modelDiameter;

        srand(time(NULL));
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
                float rand3 = ((float)rand() / (RAND_MAX)) * 2 - 1;

                float angle = 2.0f * M_PI * rand1;

                float u = rand2 + rand3;
                float r;
                if (u > 1) {
                    r = 2 - u;
                }
                else {
                    r = u;
                }
                r *= _size;

                // Convert coordinates to Cartesian
                float point1 = _pos.x + r * cos(angle);
                float point2 = _pos.y + r * sin(angle);

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
                float rand3 = ((float)rand() / (RAND_MAX)) * 2 - 1;

                float angle = 2.0f * M_PI * rand1;

                float u = rand2 + rand3;
                float r;
                if (u > 1) {
                    r = 2 - u;
                }
                else {
                    r = u;
                }
                r *= _size;

                // Convert coordinates to Cartesian
                float point1 = _pos.x + r * cos(angle);
                float point2 = _pos.y + r * sin(angle);

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
                float rand3 = ((float)rand() / (RAND_MAX)) * 2 - 1;

                float angle = 2.0f * M_PI * rand1;

                float u = rand2 + rand3;
                float r;
                if (u > 1) {
                    r = 2 - u;
                }
                else {
                    r = u;
                }
                r *= _size;

                // Convert coordinates to Cartesian
                float point1 = _pos.x + r * cos(angle);
                float point2 = _pos.y + r * sin(angle);

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

    std::vector<glm::vec2> DestructionHandler::modelVertices(const std::vector<bu::Face>* _faces, const std::weak_ptr<Transform>& _transform, ProjectionPlane _plane)
    {
        std::vector<glm::vec2> modelVertices;
        glm::vec3 pos = _transform.lock()->getPos();
        glm::vec3 scale = _transform.lock()->getScale();

        switch (_plane) // Switchs which plane I need to points from 3D vertices to 2D
        {
        case XY: // Cuts the z axis
            for (const bu::Face& face : *_faces)
            {
                modelVertices.push_back({ (face.pa.x * scale.x) + pos.x, (face.pa.y * scale.y) + pos.y });
                modelVertices.push_back({ (face.pb.x * scale.x) + pos.x, (face.pb.y * scale.y) + pos.y });
                modelVertices.push_back({ (face.pc.x * scale.x) + pos.x, (face.pc.y * scale.y) + pos.y });
            }
            break;
        case YZ: // Cuts the x axis
            for (const bu::Face& face : *_faces)
            {
                modelVertices.push_back({ (face.pa.z * scale.z) + pos.z, (face.pa.y * scale.y) + pos.y });
                modelVertices.push_back({ (face.pb.z * scale.z) + pos.z, (face.pb.y * scale.y) + pos.y });
                modelVertices.push_back({ (face.pc.z * scale.z) + pos.z, (face.pc.y * scale.y) + pos.y });
            }
            break;
        case XZ: // Cuts the y axis
            for (const bu::Face& face : *_faces)
            {
                modelVertices.push_back({ (face.pa.x * scale.x) + pos.x, (face.pa.z * scale.z) + pos.z });
                modelVertices.push_back({ (face.pb.x * scale.x) + pos.x, (face.pb.z * scale.z) + pos.z });
                modelVertices.push_back({ (face.pc.x * scale.x) + pos.x, (face.pc.z * scale.z) + pos.z });
            }
            break;
        }

        return modelVertices;
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

    glm::vec3 DestructionHandler::unProjectVertex(const glm::vec2& _point, ProjectionPlane _plane, float _savedPoint)
    {
        glm::vec3 newPoint;
        switch (_plane) // Switchs which plane I need to points from 3D vertices to 2D
        {
        case XY: // Adds the z axis
            newPoint = glm::vec3(_point.x, _point.y, _savedPoint);
            break;
        case YZ: // Adds the x axis
            newPoint = glm::vec3(_savedPoint, _point.y, _point.x);
            break;
        case XZ: // Adds the y axis
            newPoint = glm::vec3(_point.x, _savedPoint, _point.y);
            break;
        }

        return newPoint;
    }
}
