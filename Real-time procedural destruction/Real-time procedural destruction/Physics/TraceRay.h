#pragma once
#include "BaseCollider.h"

namespace GameEngine
{
	struct TraceRay
	{
		TraceRay();
		void shootRay(Ray _ray);

		void addObject(std::weak_ptr<BaseCollider> _obj)
		{
			m_objsInScene.push_back(_obj);
		}

	private:
		// Finding closest object a given ray intersects with
		intersectionInfo findClosestObject(Ray _ray);

		// List of objects in the scene
		std::vector<std::weak_ptr<BaseCollider> > m_objsInScene;

	};

}