#pragma once
#include "BaseCollider.h"
#include "..\GameEngine\Transform.h"

namespace GameEngine
{
	// MeshCollider class, has tri collider intersection maths 
	struct MeshCollider : BaseCollider
	{
	public:
		~MeshCollider();
		void initialize() override;
		void onTick() override;
		// Override function from parent object
		finalIntersection rayIntersect(Ray _ray) override; // Ray intersection with box
		bool intersectTriangle(Ray _ray,
			double vert0[3], double vert1[3], double vert2[3],
			double* t, double* u, double* v);

	private:
		// Transform pointer reference
		std::weak_ptr<GameEngine::Transform> m_transform;
	};

}