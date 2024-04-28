#include "RayTracer.h"


namespace GameEngine
{
	RayTracer::RayTracer() {}

	void RayTracer::traceRay(Ray _ray)
	{
		// Using given ray check which object has been intersected with
		finalIntersection Info = findClosestObject(_ray);

		std::cout << "HIT:" << Info.hasIntersected << std::endl;
	}

	finalIntersection RayTracer::findClosestObject(Ray _ray)
	{
		// Init values
		finalIntersection finalInfo;
		glm::vec3 closestPoint{ 0 };
		float distance = FLT_MAX;
		int objIndex = 0;
		bool checkIntersect = false;

		// Loop through each object in the scene and check if they have intersected. If intersected check if its the closest in distance and save the infomation
		for (int ei = 0; ei < m_objsInScene.size(); ei++)
		{
			finalIntersection info = m_objsInScene.at(ei).lock()->rayIntersect(_ray);

			if (info.hasIntersected)
			{
				checkIntersect = true;

				if (glm::distance(_ray.origin, info.intersectionPos) < distance)
				{
					distance = glm::distance(_ray.origin, info.intersectionPos);
					closestPoint = info.intersectionPos;
					objIndex = ei;
					finalInfo = info;
				}
			}
		}

		finalInfo.objIndex = objIndex;
		finalInfo.hasIntersected = checkIntersect;

		return finalInfo;
	}
}
