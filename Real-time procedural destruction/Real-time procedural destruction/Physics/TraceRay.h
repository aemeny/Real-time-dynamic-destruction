#pragma once
#include "BaseCollider.h"

namespace GameEngine
{
	enum ProjectionPlane;
	struct Edge;

	struct TraceRay
	{
		TraceRay(std::weak_ptr<Core> _core);
		~TraceRay();
		void shootRay(Ray _ray);
		void display();

		void addObject(BaseCollider &_obj)
		{
			m_objsInScene.push_back(&_obj);
		}

		std::vector<BaseCollider*>* getObjectsInScene() { return &m_objsInScene; }

		void stepDebugDrawBox();
		std::vector<Edge> d_vec;
		glm::vec3 d_pos;
		int d_index;
	private:
		// Finding closest object a given ray intersects with
		intersectionInfo findClosestObject(Ray _ray);

		//! Debug Components
		/*!
		   Draw Box Outline Func
		   Rendering info
		*/
		void debugDrawBox(glm::vec3 _pos, float _boxSize, ProjectionPlane _plane);
		void debugDrawBox(std::vector<Edge> _vec, glm::vec3 _pos);
		bool m_renderOutline;
		std::weak_ptr<GameEngine::LineRenderer> m_lineRenderer;
		std::weak_ptr<Renderer::Vbo> m_vbo;

		// List of objects in the scene
		std::vector<BaseCollider*> m_objsInScene;

		// Reference to Core
		std::weak_ptr<Core> m_core;
	};

}