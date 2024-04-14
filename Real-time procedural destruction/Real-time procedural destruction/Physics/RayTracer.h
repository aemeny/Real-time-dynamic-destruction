#pragma once
#include "BaseCollider.h"

namespace GameEngine
{
	struct RayTracer
	{
		RayTracer();
		void traceRay(Ray _ray);

		void addObject(std::weak_ptr<BaseCollider> _obj)
		{
			m_objsInScene.push_back(_obj);
		}

	private:
		// Finding closest object a given ray intersects with
		finalIntersection findClosestObject(Ray _ray);

		// List of objects in the scene
		std::vector<std::weak_ptr<BaseCollider> > m_objsInScene;

	};

}