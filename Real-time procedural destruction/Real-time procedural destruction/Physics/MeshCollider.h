#pragma once
#include "BaseCollider.h"

namespace GameEngine
{
	// MeshCollider class, has tri collider intersection maths 
	struct MeshCollider : BaseCollider
	{
	public:
		~MeshCollider();
		void initialize() override;
		void onTick() override;
		void drawOutline(bool _draw) override;

		// Override function from parent object
		intersectionInfo rayIntersect(Ray _ray) override; // Ray intersection with box

	private:
		std::vector<bu::Face> faces;
	};

}