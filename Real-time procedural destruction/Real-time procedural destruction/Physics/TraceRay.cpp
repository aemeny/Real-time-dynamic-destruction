#include "TraceRay.h"
#include "../GameEngine/DestructionHandler.h"

namespace GameEngine
{
	TraceRay::TraceRay(std::weak_ptr<Core> _core)
	{
		m_core = _core;
		m_renderOutline = false;
	}

	TraceRay::~TraceRay(){}

	void TraceRay::shootRay(Ray _ray)
	{
		// Using given ray check which object has been intersected with
		intersectionInfo Info = findClosestObject(_ray);
		Info.rayOrigin = _ray.origin;

		if (Info.hasIntersected)
		{
			if (Info.collidedFace != NULL)
			{
				if (Info.intersectedModel.lock()->getDestruction())
				{
					std::weak_ptr<DestructionHandler> destructionHandler = m_objsInScene[Info.objIndex]->m_entity.lock()->findComponent<DestructionHandler>();
					if (!destructionHandler.expired())
					{
						std::cout << "Destruction Sim Started\n";
						destructionHandler.lock()->destructObject(&Info, m_objsInScene[Info.objIndex]->m_entity.lock()->findComponent<Transform>());
					}
				}
			}
		}
	}

	void TraceRay::display()
	{
		if (m_renderOutline)
			m_lineRenderer.lock()->renderLine();
	}

	intersectionInfo TraceRay::findClosestObject(Ray _ray)
	{
		// Init values
		intersectionInfo finalInfo;
		float distance = FLT_MAX;
		int objIndex = 0;
		bool checkIntersect = false;

		// Loop through each object in the scene and check if they have intersected. If intersected check if its the closest in distance and save the infomation
		for (int ei = 0; ei < m_objsInScene.size(); ei++)
		{
			intersectionInfo info = m_objsInScene.at(ei)->rayIntersect(_ray);

			if (info.hasIntersected)
			{
				checkIntersect = true;

				if (info.distanceToIntersection < distance)
				{
					distance = info.distanceToIntersection;
					objIndex = ei;
					finalInfo = info;
				}
			}
		}

		finalInfo.objIndex = objIndex;
		finalInfo.distanceToIntersection = distance;
		finalInfo.hasIntersected = checkIntersect;

		return finalInfo;
	}

	void TraceRay::debugDrawBox(glm::vec3 _pos, float _boxSize, ProjectionPlane _plane)
	{
		m_renderOutline = true;
		if (m_vbo.expired())
		{
			// Attach line renderer
			std::vector<std::shared_ptr<GameEngine::LineRenderer> > lineRenderer;
			m_core.lock()->find<GameEngine::LineRenderer>(lineRenderer);
			m_lineRenderer = lineRenderer[0];

			// Create new vbo for this collider
			m_vbo = m_lineRenderer.lock()->addVbo();
		}

		// Clear the already made lines from line count + clear all data from vbo list
		m_lineRenderer.lock()->clearLines(m_vbo);

		// Add positions
		float halfBoxSize = _boxSize * 0.5f;
		glm::vec3 pos1;
		glm::vec3 pos2;
		glm::vec3 pos3;
		glm::vec3 pos4;
		glm::vec3 pos5;
		glm::vec3 pos6;
		glm::vec3 pos7;
		glm::vec3 pos8;

		switch (_plane) // Switchs which plane I need to project the models 3D vertices to 2D
		{
		case XY: // Cuts the z axis
			pos1 = glm::vec3(_pos.x + halfBoxSize, _pos.y + halfBoxSize, _pos.z);
			pos2 = glm::vec3(_pos.x + halfBoxSize, _pos.y + halfBoxSize, _pos.z);
			pos3 = glm::vec3(_pos.x + halfBoxSize, _pos.y - halfBoxSize, _pos.z );
			pos4 = glm::vec3(_pos.x - halfBoxSize, _pos.y + halfBoxSize, _pos.z);
			pos5 = glm::vec3(_pos.x - halfBoxSize, _pos.y - halfBoxSize, _pos.z);
			pos6 = glm::vec3(_pos.x - halfBoxSize, _pos.y - halfBoxSize, _pos.z);
			pos7 = glm::vec3(_pos.x - halfBoxSize, _pos.y + halfBoxSize, _pos.z);
			pos8 = glm::vec3(_pos.x + halfBoxSize, _pos.y - halfBoxSize, _pos.z);
			break;
		case YZ: // Cuts the x axis
			pos1 = glm::vec3(_pos.x, _pos.y + halfBoxSize, _pos.z + halfBoxSize);
			pos2 = glm::vec3(_pos.x, _pos.y + halfBoxSize, _pos.z - halfBoxSize);
			pos3 = glm::vec3(_pos.x, _pos.y - halfBoxSize, _pos.z + halfBoxSize);
			pos4 = glm::vec3(_pos.x, _pos.y + halfBoxSize, _pos.z + halfBoxSize);
			pos5 = glm::vec3(_pos.x, _pos.y - halfBoxSize, _pos.z - halfBoxSize);
			pos6 = glm::vec3(_pos.x, _pos.y - halfBoxSize, _pos.z + halfBoxSize);
			pos7 = glm::vec3(_pos.x, _pos.y + halfBoxSize, _pos.z - halfBoxSize);
			pos8 = glm::vec3(_pos.x, _pos.y - halfBoxSize, _pos.z - halfBoxSize);
			break;
		case XZ: // Cuts the y axis
			pos1 = glm::vec3(_pos.x + halfBoxSize, _pos.y, _pos.z + halfBoxSize);
			pos2 = glm::vec3(_pos.x + halfBoxSize, _pos.y, _pos.z - halfBoxSize);
			pos3 = glm::vec3(_pos.x + halfBoxSize, _pos.y, _pos.z + halfBoxSize);
			pos4 = glm::vec3(_pos.x - halfBoxSize, _pos.y, _pos.z + halfBoxSize);
			pos5 = glm::vec3(_pos.x - halfBoxSize, _pos.y, _pos.z - halfBoxSize);
			pos6 = glm::vec3(_pos.x - halfBoxSize, _pos.y, _pos.z + halfBoxSize);
			pos7 = glm::vec3(_pos.x - halfBoxSize, _pos.y, _pos.z - halfBoxSize);
			pos8 = glm::vec3(_pos.x + halfBoxSize, _pos.y, _pos.z - halfBoxSize);
			break;
		}

		m_lineRenderer.lock()->addLine(m_vbo, pos1, pos2);
		m_lineRenderer.lock()->addLine(m_vbo, pos1, pos3);
		m_lineRenderer.lock()->addLine(m_vbo, pos1, pos4);
		m_lineRenderer.lock()->addLine(m_vbo, pos5, pos6);
		m_lineRenderer.lock()->addLine(m_vbo, pos5, pos7);
		m_lineRenderer.lock()->addLine(m_vbo, pos5, pos8);
		m_lineRenderer.lock()->addLine(m_vbo, pos6, pos3);
		m_lineRenderer.lock()->addLine(m_vbo, pos8, pos3);
		m_lineRenderer.lock()->addLine(m_vbo, pos8, pos2);
		m_lineRenderer.lock()->addLine(m_vbo, pos7, pos2);
		m_lineRenderer.lock()->addLine(m_vbo, pos7, pos4);
		m_lineRenderer.lock()->addLine(m_vbo, pos6, pos4);
	}

	void TraceRay::debugDrawBox(std::vector<Edge> _vec, glm::vec3 _pos)
	{
		m_renderOutline = true;

		if (m_vbo.expired())
		{
			// Attach line renderer
			std::vector<std::shared_ptr<GameEngine::LineRenderer> > lineRenderer;
			m_core.lock()->find<GameEngine::LineRenderer>(lineRenderer);
			m_lineRenderer = lineRenderer[0];

			// Create new vbo for this collider
			m_vbo = m_lineRenderer.lock()->addVbo();
		}

		// Clear the already made lines from line count + clear all data from vbo list
		m_lineRenderer.lock()->clearLines(m_vbo);

		/*for (int i = 0; i < _vec.size(); i++)
		{
			glm::vec3 pos1(_vec[i].m_vertices[0].x, _vec[i].m_vertices[0].y, _pos.z + 0.02f);
			glm::vec3 pos2(_vec[i].m_vertices[1].x, _vec[i].m_vertices[1].y, _pos.z + 0.02f);
			glm::vec3 pos3(_vec[i].m_vertices[2].x, _vec[i].m_vertices[2].y, _pos.z + 0.02f);

			m_lineRenderer.lock()->addLine(m_vbo, pos1, pos2);
			m_lineRenderer.lock()->addLine(m_vbo, pos2, pos3);
			m_lineRenderer.lock()->addLine(m_vbo, pos3, pos1);
		}*/

		/*for (int i = 0; i < _cells.size(); i++)
		{
			for (int j = 0; j < _cells[i].m_edges.size(); j++)
			{
				glm::vec3 pos1(_cells[i].m_edges[j].m_start.x, _cells[i].m_edges[j].m_start.y, _cells[i].m_edges[j].m_start.z + 0.02f);
				glm::vec3 pos2(_cells[i].m_edges[j].m_end.x, _cells[i].m_edges[j].m_end.y, _cells[i].m_edges[j].m_start.z + 0.02f);

				m_lineRenderer.lock()->addLine(m_vbo, pos1, pos2);
			}
		}*/

		bool holeOutline = false;
		for (int i = 0; i < _vec.size(); i++)
		{
			if (_vec[i].m_start == glm::vec2(0) && _vec[i].m_end == glm::vec2(0))
			{
				holeOutline = true;
			}
			glm::vec3 pos1;
			glm::vec3 pos2;
			if (holeOutline)
			{
				pos1 = glm::vec3(_vec[i].m_start.x, _vec[i].m_start.y, _pos.z + 0.32f);
				pos2 = glm::vec3(_vec[i].m_end.x, _vec[i].m_end.y, _pos.z + 0.32f);
			}
			else
			{
				pos1 = glm::vec3(_vec[i].m_start.x, _vec[i].m_start.y, _pos.z + 0.02f);
				pos2 = glm::vec3(_vec[i].m_end.x, _vec[i].m_end.y, _pos.z + 0.02f);
			}
			

			m_lineRenderer.lock()->addLine(m_vbo, pos1, pos2);
		}
	}

	void TraceRay::stepDebugDrawBox()
	{
		m_renderOutline = true;

		if (m_vbo.expired())
		{
			// Attach line renderer
			std::vector<std::shared_ptr<GameEngine::LineRenderer> > lineRenderer;
			m_core.lock()->find<GameEngine::LineRenderer>(lineRenderer);
			m_lineRenderer = lineRenderer[0];

			// Create new vbo for this collider
			m_vbo = m_lineRenderer.lock()->addVbo();
		}

		//m_lineRenderer.lock()->clearLines(m_vbo);

		//std::cout << "Edges:\n";
		//std::cout << d_tris[d_index].m_edges.size() << std::endl;


		/*glm::vec3 pos1(d_vec[d_index].m_vertices[0].x, d_vec[d_index].m_vertices[0].y, d_pos.z + 0.02f);
		glm::vec3 pos2(d_vec[d_index].m_vertices[1].x, d_vec[d_index].m_vertices[1].y, d_pos.z + 0.02f);
		glm::vec3 pos3(d_vec[d_index].m_vertices[2].x, d_vec[d_index].m_vertices[2].y, d_pos.z + 0.02f);

		m_lineRenderer.lock()->addLine(m_vbo, pos1, pos2);
		m_lineRenderer.lock()->addLine(m_vbo, pos2, pos3);
		m_lineRenderer.lock()->addLine(m_vbo, pos3, pos1);*/


		glm::vec3 pos1(d_vec[d_index].m_start.x, d_vec[d_index].m_start.y, d_pos.z + 0.22f);
		glm::vec3 pos2(d_vec[d_index].m_end.x, d_vec[d_index].m_end.y, d_pos.z + 0.22f);

		m_lineRenderer.lock()->addLine(m_vbo, pos1, pos2);


		/*for (int j = 0; j < d_cells[d_index].m_edges.size(); j++)
		{
			glm::vec3 pos1(d_cells[d_index].m_edges[j].m_start.x, d_cells[d_index].m_edges[j].m_start.y, d_cells[d_index].m_edges[j].m_start.z + 0.5f);
			glm::vec3 pos2(d_cells[d_index].m_edges[j].m_end.x, d_cells[d_index].m_edges[j].m_end.y, d_cells[d_index].m_edges[j].m_start.z + 0.5f);

			m_lineRenderer.lock()->addLine(m_vbo, pos1, pos2);
		}*/

		d_index++;
		if (d_index == d_vec.size())
		{
			d_index = 0;
		}
	}
}
