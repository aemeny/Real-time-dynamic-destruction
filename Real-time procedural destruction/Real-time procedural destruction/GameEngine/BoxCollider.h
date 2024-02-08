#pragma once
#include "Component.h"

namespace GameEngine
{
	/*
	*  BoxCollider struct inheriting from component,checks for collision and gives a response
	*/
	struct BoxCollider : Component
	{
		BoxCollider();
		~BoxCollider();

		void onTick() override; // Checks through all other box colliders and check if they are intersecting
		bool isCollidingCheck(std::shared_ptr<BoxCollider> _other); // Compares current box collider versus another to check intersection
		void getCollisionResponse(std::shared_ptr<BoxCollider> _other); // If colliding respond currently by moving out the other colliders radius

		void setSize(glm::vec3 _size) { m_size = _size; } // Setter for colliser size
		void setStaticObject(bool _set) { staticObject = _set; } // Setter to check if the object is static (doesnt respond)
		void setOffset(glm::vec3 _offset) { m_offset = _offset; } // Setter for offset to the hitbox

		bool isColliding; // If the collider is collising on this frame
	private:
		// Variables
		glm::vec3 m_size;
		glm::vec3 m_offset;
		bool staticObject;
	};
}

