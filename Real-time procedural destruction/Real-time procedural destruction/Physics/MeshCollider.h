#pragma once
#include "..\GameEngine\ModelRenderer.h"
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
		void onDisplay() override;
		void updateOutline() override;

		// Override function from parent object
		intersectionInfo rayIntersect(Ray _ray) override; // Ray intersection with box		
		std::weak_ptr<GameEngine::ModelRenderer> m_modelRenderer;
	};

}