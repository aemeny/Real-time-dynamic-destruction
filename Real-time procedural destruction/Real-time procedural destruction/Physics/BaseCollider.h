#pragma once
#include "..\GameEngine\Component.h"
#include "Ray.h"

namespace GameEngine
{
	// Struct for intersect value information 
	struct finalIntersection
	{
		glm::vec3 intersectionPos{ 0,0,0 };
		glm::vec3 surfaceNormal{ 0,0,0 };
		bool hasIntersected{ false };
		int objIndex = 0;
	};

	struct BaseCollider : Component
	{
		// Virtual function to be overrided by children
		virtual finalIntersection rayIntersect(Ray _ray);
		void setColliderOffset(glm::vec3 _offset) { m_colliderOffset = _offset; }

	private:
		glm::vec3 m_colliderOffset;
	};

}