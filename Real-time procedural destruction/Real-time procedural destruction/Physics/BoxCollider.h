#pragma once
#include "BaseCollider.h"


namespace GameEngine
{
	// BoxCollider class, has box collider intersection maths 
	struct BoxCollider : BaseCollider
	{
	public:
		~BoxCollider();
		void initialize() override;
		void onTick() override;
		void drawOutline(bool _draw) override;

		// Override function from parent object
		intersectionInfo rayIntersect(Ray _ray) override; // Ray intersection with box

		void setColliderSize(glm::vec3 _size) { m_colliderSize = _size; }

	private:
		// Transform pointer reference
		std::weak_ptr<GameEngine::Transform> m_transform;
		glm::vec3 m_colliderSize;
	};

}