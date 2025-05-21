#include "DestructionHandler.h"
#include "ModelRenderer.h"
#include "../Physics/TraceRay.h"
#include "../Physics/RigidBody.h"
#include "../Project/DestructedObject.h"

#include <glm/gtx/normal.hpp>
#include <random>

namespace GameEngine
{
    DestructionHandler::~DestructionHandler() {}

    DestructionHandler::DestructionHandler()
    {
        srand(time(NULL)); // Sets random seed
        m_randomFromCircle = false;
        m_destructionDiameter = 0;
        m_pointGenerateDepth = 0;
    }

    void DestructionHandler::destructObject(intersectionInfo* _info, std::weak_ptr<Transform> _transform)
    {
        // Use normal vector of the collided face to determine projection
        ProjectionPlane plane = determineProjectionPlane(_info->collidedFace);

        // Generates a set number of points randomly in a 2D square aligned to the faces normal
        std::vector<glm::vec3> generatedPoints = generateRandomPoints(_info->intersectionPos, plane, _transform); //Intersect pos, plane to project, model transform

        // Create a delaunay diagram from the new generated points, after projecting them into 2D
        float savedUnprojetedPoint;
        Delaunay delaunayDiagram = Delaunay(projectVertices(&generatedPoints, plane, savedUnprojetedPoint));

        // Create Voronoi diagram from generate delaunay triangles
        VoronoiDiagram voronoiDiagram;
        voronoiDiagram.generate(delaunayDiagram.m_triangles);

        // Cut edges to bounds of model that go out of area
        LineClippingAlgorithm lineClipper = LineClippingAlgorithm(_transform, plane);
        cutEdges(&voronoiDiagram.m_voronoiCells, &lineClipper);



        // Debug line renderer
        std::vector<std::shared_ptr<LineRenderer> > lineRenderer;
        core().lock()->find<LineRenderer>(lineRenderer);
        std::weak_ptr<Renderer::Vbo> vbo = lineRenderer[0]->addVbo();

        /* -- Generate fragment pieces based on the voronoi diagram -- */   
        for (VoronoiCell cell : voronoiDiagram.m_voronoiCells)
        {
            /* --- Make new entity for this cell --- */
            std::shared_ptr<Entity> newCellEntity = core().lock()->addEntity();
            std::weak_ptr<Transform> cellTransform = newCellEntity->m_transform;
            newCellEntity->addComponent<DestructedObject>();
            std::weak_ptr<RigidBody> cellRigidBody = newCellEntity->addComponent<RigidBody>();
            std::weak_ptr<ModelRenderer> modelRenderer = newCellEntity->addComponent<ModelRenderer>();
            modelRenderer.lock()->setModel("Cube/Cube.obj", true);
            modelRenderer.lock()->setTexture("Floor/CustomUV.png");

            /*  --- Trianglulate new cell --- */
            // Add all vertices to list
            std::vector<glm::vec2> cellVertices;
            for (const Edge& edge : cell.m_edges)
            {
                cellVertices.push_back(edge.m_start);
                cellVertices.push_back(edge.m_end);
            }
            // Remove duplicates
            std::sort(cellVertices.begin(), cellVertices.end(), [](const glm::vec2& a, const glm::vec2& b) {
                if (a.x != b.x) return a.x < b.x; // Compare x components
                return a.y < b.y; // Compare y components
                });
            auto last = std::unique(cellVertices.begin(), cellVertices.end());
            cellVertices.erase(last, cellVertices.end());

            // Create delaunay from points
            Delaunay localCellDelaunay = Delaunay(cellVertices);

            // Create new faces
            float customZDepth = _transform.lock()->getPos().z - _transform.lock()->getScale().z;
            std::vector<bu::Face> newCellFaces;
            addNewFaces(&newCellFaces, localCellDelaunay.m_triangles, cellTransform, plane, savedUnprojetedPoint, customZDepth);

            // Connect faces
            std::vector<std::vector<glm::vec2> > orderedEdgePoints = localCellDelaunay.getOrderedTrianglePoints(cell.m_edges);
            connectFaces(&newCellFaces, orderedEdgePoints, cellTransform, plane, savedUnprojetedPoint, customZDepth);

            // Update model
            std::weak_ptr<Renderer::Model> model = modelRenderer.lock()->getModel();
            model.lock()->setVertices(newCellFaces.size() * 3);
            model.lock()->setFaces(newCellFaces);
            model.lock()->updateModel();
            model.lock()->setDestruction(false);
            cellTransform.lock()->setDirty(true);

            // Apply physics
            // Calculate center of the cell
            glm::vec2 centroid(0.0f, 0.0f);
            for (const glm::vec2& vertex : cellVertices) {
                centroid += vertex;
            }
            centroid /= cellVertices.size();

            // Calculate exposion point velocity impact
            glm::vec3 diff = glm::vec3(centroid, 0) - _info->intersectionPos;
            // Increase Strength
            diff *= 3;

            cellRigidBody.lock()->addImpulse(glm::vec3(diff.x, 2 + diff.y, -3));
        }


        // Generate a convex hull cell for the orignal squares cutout
        VoronoiCell convexHull = voronoiDiagram.generateConvexHull(voronoiDiagram.m_voronoiCells);

        // Grab faces of model with convex hull point to retriangulate
        std::vector<bu::Face>* faces = _info->intersectedModel.lock()->getFaces();
        Delaunay newModelTriangles = Delaunay(modelVertices(faces, _transform, plane), convexHull.m_edges);

        // Remove old faces and add the new ones
        std::vector<bu::Face> newFaces;
        addNewFaces(&newFaces, newModelTriangles.m_triangles, _transform, plane, savedUnprojetedPoint);

        // Grab outerEdges of the model
        std::vector<Edge> outerEdges;
        for (const bu::Face& face : *faces)
        {
            outerEdges.push_back(Edge(face.pa, face.pb));
            outerEdges.push_back(Edge(face.pb, face.pc));
            outerEdges.push_back(Edge(face.pc, face.pa));
        }
        // Connect the front face to the back face by filling the hole in the mesh with new triangles
        std::vector<std::vector<glm::vec2> > orderedEdgePoints = newModelTriangles.getOrderedTrianglePoints(outerEdges);
        connectFaces(&newFaces, orderedEdgePoints, _transform, plane, savedUnprojetedPoint);

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
            return ProjectionPlane::YZ;  // Normal is aligned to the x axis, project onto YZ plane
        else if (absY > absZ) 
            return ProjectionPlane::XZ;  // Normal is aligned to the y axis, project onto XZ plane
        else 
            return ProjectionPlane::XY;  // Normal is aligned to the z axis, project onto XY plane
    }

    std::vector<glm::vec3> DestructionHandler::generateRandomPoints(glm::vec3 _pos, ProjectionPlane _plane, std::weak_ptr<Transform> _transform)
    {
        std::vector<glm::vec3> points;
        float halfSize;
        glm::vec3 modelPos = _transform.lock()->getPos();
        glm::vec2 modelDiameter;

        /*
           Looks to make sure there are reasonanal amount of points to destruct before continuing
           After depth is ended, try to aim for atleast 5 points in given area, to decrease loops / computation time minimum needed decreases every loop
        */
        int minimumThreashold = m_pointGenerateDepth + 5;
        int i = 0;
        switch (_plane) // Switchs which plane I need to keep impact position
        {
        case XY: // Cuts the z axis
            modelDiameter = glm::vec2(_transform.lock()->getScale().x, _transform.lock()->getScale().y);
            m_destructionDiameter = m_destructionDiameter < modelDiameter.x && m_destructionDiameter < modelDiameter.y ? m_destructionDiameter : m_destructionDiameter > modelDiameter.x ? modelDiameter.x : modelDiameter.y;

            while (i < m_pointGenerateDepth || points.size() < minimumThreashold)
            {
                glm::vec2 point(0);
                if (m_randomFromCircle)
                    point = randomPointInCircle(glm::vec2(_pos.x, _pos.y));
                else
                    point = randomPointInSquare(glm::vec2(_pos.x, _pos.y));

                if (glm::abs(point.x - modelPos.x) < modelDiameter.x && glm::abs(point.y - modelPos.y) < modelDiameter.y) // If in bounds of model
                {
                    points.push_back(glm::vec3(point, _pos.z));
                }
                i++;
                if (minimumThreashold > 5) // Makes sure atleast one point is created in area before continuing
                    minimumThreashold--; //Stops loop taking too much time if not found quick enough
                if (i > 100 + m_pointGenerateDepth)
                {
                    std::cout << "Too many checks, broke out point creation! \n";  
                    break;
                }
            }
            break;
        case YZ: // Cuts the x axis
            modelDiameter = glm::vec2(_transform.lock()->getScale().y, _transform.lock()->getScale().z);
            m_destructionDiameter = m_destructionDiameter < modelDiameter.x && m_destructionDiameter < modelDiameter.y ? m_destructionDiameter : m_destructionDiameter > modelDiameter.x ? modelDiameter.x : modelDiameter.y;
            
            while (i < m_pointGenerateDepth || points.size() < minimumThreashold)
            {
                glm::vec2 point(0);
                if (m_randomFromCircle)
                    point = randomPointInCircle(glm::vec2(_pos.y, _pos.z));
                else
                    point = randomPointInSquare(glm::vec2(_pos.y, _pos.z));

                if (glm::abs(point.x - modelPos.y) < modelDiameter.x && glm::abs(point.y - modelPos.z) < modelDiameter.y) // If in bounds of model
                {
                    points.push_back(glm::vec3(_pos.x, point));
                }
                i++;
                if (minimumThreashold > 5)
                    minimumThreashold--;
                if (i > 100)
                {
                    std::cout << "Too many checks, broke out point creation! \n";
                    break;
                }
            }
            break;
        case XZ: // Cuts the y axis
            modelDiameter = glm::vec2(_transform.lock()->getScale().x, _transform.lock()->getScale().z);
            m_destructionDiameter = m_destructionDiameter < modelDiameter.x && m_destructionDiameter < modelDiameter.y ? m_destructionDiameter : m_destructionDiameter > modelDiameter.x ? modelDiameter.x : modelDiameter.y;

            while (i < m_pointGenerateDepth || points.size() < minimumThreashold)
            {
                glm::vec2 point(0);
                if (m_randomFromCircle)
                    point = randomPointInCircle(glm::vec2(_pos.x, _pos.z));
                else
                    point = randomPointInSquare(glm::vec2(_pos.x, _pos.z));

                if (glm::abs(point.x - modelPos.x) < modelDiameter.x && glm::abs(point.y - modelPos.z) < modelDiameter.y) // If in bounds of model
                {
                    points.push_back(glm::vec3(point.x, _pos.y, point.y));
                }
                i++;
                if (minimumThreashold > 5)
                    minimumThreashold--;
                if (i > 100)
                {
                    std::cout << "Too many checks, broke out point creation! \n";
                    break;
                }
            }
            break;
        }

        points.push_back(glm::vec3(_pos)); // Always have a point on impact
        return points;
    }

    glm::vec2 DestructionHandler::randomPointInSquare(glm::vec2 _pos)
    {
        glm::vec2 point;
        float randNum1 = ((float)rand() / (RAND_MAX)) * 2 - 1;
        float randNum2 = ((float)rand() / (RAND_MAX)) * 2 - 1;
        point.x = _pos.x + (randNum1 * (m_destructionDiameter * 0.5f));
        point.y = _pos.y + (randNum2 * (m_destructionDiameter * 0.5f));
        return point;
    }

    glm::vec2 DestructionHandler::randomPointInCircle(glm::vec2 _pos)
    {
        glm::vec2 point;
        float randNum1 = ((float)rand() / (RAND_MAX)) * 2 - 1;
        float randNum2 = ((float)rand() / (RAND_MAX)) * 2 - 1;
        float randNum3 = ((float)rand() / (RAND_MAX)) * 2 - 1;

        float angle = 2.0f * M_PI * randNum1;
        float u = randNum2 + randNum3;
        float r;
        if (u > 1) 
            r = 2 - u;
        else 
            r = u;
        r *= (m_destructionDiameter * 0.5f);

        // Convert coordinates to Cartesian
        point.x = _pos.x + r * cos(angle);
        point.y = _pos.y + r * sin(angle);
        return point;
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

    void DestructionHandler::cutEdges(std::vector<VoronoiCell>* _voronoiCells, LineClippingAlgorithm* _lineClipper)
    {
        std::vector<Edge> connectingEdges;
        for (VoronoiCell& cell : *_voronoiCells)
        {
            std::vector<Edge> cutEdges;
            for (Edge& edge : cell.m_edges)
            {
                Edge clippedEdge = _lineClipper->CohenSutherland(edge);
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
    }

    void DestructionHandler::addNewFaces(std::vector<bu::Face>* _newFaces, const std::vector<Triangle>& _tris, const std::weak_ptr<Transform>& _transform, ProjectionPlane _plane, float _savedPoint, float _customDepth)
    {
        glm::vec3 pos = _transform.lock()->getPos();
        glm::vec3 scale = _transform.lock()->getScale();

        for (const Triangle& tri : _tris)
        {
            for (int j = 0; j < 2; j++)
            {
                bu::Face newFace;
                if (j == 0)
                {
                    newFace.pa = unProjectVertex({ (tri.m_vertices[0].x - pos.x) / scale.x, (tri.m_vertices[0].y - pos.y) / scale.y }, _plane, (_savedPoint - pos.z) / scale.z);
                    newFace.pb = unProjectVertex({ (tri.m_vertices[1].x - pos.x) / scale.x, (tri.m_vertices[1].y - pos.y) / scale.y }, _plane, (_savedPoint - pos.z) / scale.z);
                    newFace.pc = unProjectVertex({ (tri.m_vertices[2].x - pos.x) / scale.x, (tri.m_vertices[2].y - pos.y) / scale.y }, _plane, (_savedPoint - pos.z) / scale.z);
                    switch (_plane)
                    {
                    case XY:
                        newFace.na = newFace.nb = newFace.nc = glm::vec3(0, 0, 1);
                        break;
                    case YZ:
                        newFace.na = newFace.nb = newFace.nc = glm::vec3(1, 0, 0);
                        break;
                    case XZ: // Cuts the y axis
                        newFace.na = newFace.nb = newFace.nc = glm::vec3(0, 1, 0);
                        break;
                    }
                }
                else
                {
                    float zPos;
                    if (_customDepth != NULL)
                        zPos = _customDepth;
                    else
                        zPos = ((_savedPoint - scale.z * 2.0f) - pos.z) / scale.z;
                    
                    newFace.pa = unProjectVertex({ (tri.m_vertices[0].x - pos.x) / scale.x, (tri.m_vertices[0].y - pos.y) / scale.y }, _plane, zPos);
                    newFace.pb = unProjectVertex({ (tri.m_vertices[1].x - pos.x) / scale.x, (tri.m_vertices[1].y - pos.y) / scale.y }, _plane, zPos);
                    newFace.pc = unProjectVertex({ (tri.m_vertices[2].x - pos.x) / scale.x, (tri.m_vertices[2].y - pos.y) / scale.y }, _plane, zPos);
                    switch (_plane)
                    {
                    case XY:
                        newFace.na = newFace.nb = newFace.nc = glm::vec3(0, 0, -1);
                        break;
                    case YZ:
                        newFace.na = newFace.nb = newFace.nc = glm::vec3(-1, 0, 0);
                        break;
                    case XZ: // Cuts the y axis
                        newFace.na = newFace.nb = newFace.nc = glm::vec3(0, -1, 0);
                        break;
                    }
                }

                // Assign vertex positions
                float scaleSize = 1.0f / 1.3f;
                newFace.tca = glm::vec2(newFace.pa.x * scale.x * scaleSize, newFace.pa.y * scale.y * scaleSize);
                newFace.tcb = glm::vec2(newFace.pb.x * scale.x * scaleSize, newFace.pb.y * scale.y * scaleSize);
                newFace.tcc = glm::vec2(newFace.pc.x * scale.x * scaleSize, newFace.pc.y * scale.y * scaleSize);

                newFace.lmca = newFace.lmcb = newFace.lmcc = glm::vec2(0);

                // Add the new face to the list of faces
                _newFaces->push_back(newFace);
            }
        }
    }

    void DestructionHandler::connectFaces(std::vector<bu::Face>* _newFaces, std::vector<std::vector<glm::vec2>>& _edgePoints, const std::weak_ptr<Transform>& _transform, ProjectionPlane _plane, float _savedPoint, float _customDepth)
    {
        glm::vec3 pos = _transform.lock()->getPos();
        glm::vec3 scale = _transform.lock()->getScale();
        for (int i = 0; i < _edgePoints.size(); i++) // For each vector in vectors (disconnected parts of model)
        {
            for (int j = 0; j < _edgePoints[i].size(); j++) // Each vertex in a loop
            {
                for (int k = 0; k < 2; k++)
                {
                    int nextPoint;
                    if (j == _edgePoints[i].size() - 1)
                        nextPoint = 0;
                    else
                        nextPoint = j + 1;

                    bu::Face newFace;
                    if (k == 0)
                    {
                        // Assign vertex positions
                        if (_customDepth == NULL)
                        {
                            newFace.pa = unProjectVertex({ (_edgePoints[i][j].x - pos.x) / scale.x, (_edgePoints[i][j].y - pos.y) / scale.y }, _plane, (_savedPoint - pos.z) / scale.z);
                            newFace.pb = unProjectVertex({ (_edgePoints[i][nextPoint].x - pos.x) / scale.x, (_edgePoints[i][nextPoint].y - pos.y) / scale.y }, _plane, ((_savedPoint - scale.z * 2.0f) - pos.z) / scale.z);
                            newFace.pc = unProjectVertex({ (_edgePoints[i][j].x - pos.x) / scale.x, (_edgePoints[i][j].y - pos.y) / scale.y }, _plane, ((_savedPoint - scale.z * 2.0f) - pos.z) / scale.z);
                        }
                        else
                        {
                            newFace.pa = unProjectVertex({ (_edgePoints[i][j].x - pos.x) / scale.x, (_edgePoints[i][j].y - pos.y) / scale.y }, _plane, -_customDepth);
                            newFace.pb = unProjectVertex({ (_edgePoints[i][nextPoint].x - pos.x) / scale.x, (_edgePoints[i][nextPoint].y - pos.y) / scale.y }, _plane, _customDepth);
                            newFace.pc = unProjectVertex({ (_edgePoints[i][j].x - pos.x) / scale.x, (_edgePoints[i][j].y - pos.y) / scale.y }, _plane, _customDepth);
                        }
                    }
                    else
                    {
                        // Assign vertex positions
                        if (_customDepth == NULL)
                        {
                            newFace.pa = unProjectVertex({ (_edgePoints[i][j].x - pos.x) / scale.x, (_edgePoints[i][j].y - pos.y) / scale.y }, _plane, (_savedPoint - pos.z) / scale.z);
                            newFace.pb = unProjectVertex({ (_edgePoints[i][nextPoint].x - pos.x) / scale.x, (_edgePoints[i][nextPoint].y - pos.y) / scale.y }, _plane, (_savedPoint - pos.z) / scale.z);
                            newFace.pc = unProjectVertex({ (_edgePoints[i][nextPoint].x - pos.x) / scale.x, (_edgePoints[i][nextPoint].y - pos.y) / scale.y }, _plane, ((_savedPoint - scale.z * 2.0f) - pos.z) / scale.z);
                        }
                        else
                        {
                            newFace.pa = unProjectVertex({ (_edgePoints[i][j].x - pos.x) / scale.x, (_edgePoints[i][j].y - pos.y) / scale.y }, _plane, -_customDepth);
                            newFace.pb = unProjectVertex({ (_edgePoints[i][nextPoint].x - pos.x) / scale.x, (_edgePoints[i][nextPoint].y - pos.y) / scale.y }, _plane, -_customDepth);
                            newFace.pc = unProjectVertex({ (_edgePoints[i][nextPoint].x - pos.x) / scale.x, (_edgePoints[i][nextPoint].y - pos.y) / scale.y }, _plane, _customDepth);
                        }
                    }

                    // Find the normal for the new face given the three points
                    glm::vec3 normal = glm::normalize(glm::triangleNormal(newFace.pa, newFace.pb, newFace.pc));
                    newFace.na = newFace.nb = newFace.nc = normal;
                    glm::vec3 absNorm = glm::abs(normal);

                    // Set new texture coords for the face based on the normal
                    float scaleSize = 1.0f / 1.3f;
                    if (absNorm.x > absNorm.y && absNorm.x > absNorm.z)
                    {
                        newFace.tca = glm::vec2(newFace.pa.y * scale.y * scaleSize, newFace.pa.z * scale.z * scaleSize);
                        newFace.tcb = glm::vec2(newFace.pb.y * scale.y * scaleSize, newFace.pb.z * scale.z * scaleSize);
                        newFace.tcc = glm::vec2(newFace.pc.y * scale.y * scaleSize, newFace.pc.z * scale.z * scaleSize);
                    }
                    else if (absNorm.y > absNorm.x && absNorm.y > absNorm.z)
                    {
                        newFace.tca = glm::vec2(newFace.pa.x * scale.x * scaleSize, newFace.pa.z * scale.z * scaleSize);
                        newFace.tcb = glm::vec2(newFace.pb.x * scale.x * scaleSize, newFace.pb.z * scale.z * scaleSize);
                        newFace.tcc = glm::vec2(newFace.pc.x * scale.x * scaleSize, newFace.pc.z * scale.z * scaleSize);
                    }
                    else
                    {
                        newFace.tca = glm::vec2(newFace.pa.x * scale.x * scaleSize, newFace.pa.y * scale.y * scaleSize);
                        newFace.tcb = glm::vec2(newFace.pb.x * scale.x * scaleSize, newFace.pb.y * scale.y * scaleSize);
                        newFace.tcc = glm::vec2(newFace.pc.x * scale.x * scaleSize, newFace.pc.y * scale.y * scaleSize);
                    }

                    newFace.lmca = newFace.lmcb = newFace.lmcc = glm::vec2(0); // Default value

                    // Add the new face to the list of faces
                    _newFaces->push_back(newFace);
                }
            }
        }
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