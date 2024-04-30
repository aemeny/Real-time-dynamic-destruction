#include "TraceRay.h"

namespace GameEngine
{
	TraceRay::TraceRay() {}

	void TraceRay::shootRay(Ray _ray)
	{
		// Using given ray check which object has been intersected with
		intersectionInfo Info = findClosestObject(_ray);

		if (Info.hasIntersected)
		{
			std::cout << "OBJ:" << Info.objIndex << std::endl;
			std::cout << "DIS:" << Info.distanceToIntersection << std::endl;
		}
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
}
