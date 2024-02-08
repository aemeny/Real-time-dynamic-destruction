#include "BoxCollider.h"
#include "Transform.h"

namespace GameEngine
{
	/*
   * constructor
   */
	BoxCollider::BoxCollider() : m_size(2.0f), m_offset(0.0f), isColliding(false), staticObject(true)
	{}

	BoxCollider::~BoxCollider() {}

	/*
   * onTick() checks through each box collider to see if they are intersecting and then responsed accordingly
   */
	void BoxCollider::onTick()
	{
		isColliding = false;
		std::vector<std::shared_ptr<BoxCollider> > boxColliders;
		core().lock()->find<BoxCollider>(boxColliders);

		for each (std::shared_ptr<BoxCollider> otherCollider in boxColliders)
		{
			if (otherCollider->m_entity.lock() != m_entity.lock()) // Dont want to collide with self
			{
				if (isCollidingCheck(otherCollider)) {
					isColliding = true;
					if (!staticObject)
					{
						getCollisionResponse(otherCollider);
					}
				}
			}
		}
	}

	/*
   * getCollisionResponse() moves the entitys transform until they are no longer colliding with the other entity by 
   * moving out in each direction until they are no longer colliding
   */
	void BoxCollider::getCollisionResponse(std::shared_ptr<BoxCollider> _other)
	{
		float amount = 0.1f;
		float step = 0.1f;
		glm::vec3 currentPos = transform().lock()->getPos();

		while (true)
		{
			if (!isCollidingCheck(_other)) break;
			transform().lock()->setPos(glm::vec3(currentPos.x + amount, currentPos.y, currentPos.z));
			if (!isCollidingCheck(_other)) break;
			transform().lock()->setPos(glm::vec3(currentPos.x - amount, currentPos.y, currentPos.z));
			transform().lock()->setPos(glm::vec3(currentPos.x - amount, currentPos.y, currentPos.z));
			if (!isCollidingCheck(_other)) break;
			transform().lock()->setPos(glm::vec3(currentPos.x + amount, currentPos.y, currentPos.z));
			transform().lock()->setPos(glm::vec3(currentPos.x, currentPos.y, currentPos.z + amount));
			if (!isCollidingCheck(_other)) break;
			transform().lock()->setPos(glm::vec3(currentPos.x, currentPos.y, currentPos.z - amount));
			transform().lock()->setPos(glm::vec3(currentPos.x, currentPos.y, currentPos.z - amount));
			if (!isCollidingCheck(_other)) break;
			transform().lock()->setPos(glm::vec3(currentPos.x, currentPos.y, currentPos.z + amount));
			transform().lock()->setPos(glm::vec3(currentPos.x, currentPos.y + amount, currentPos.z));
			if (!isCollidingCheck(_other)) break;
			transform().lock()->setPos(glm::vec3(currentPos.x, currentPos.y - amount, currentPos.z));
			transform().lock()->setPos(glm::vec3(currentPos.x, currentPos.y - amount, currentPos.z));
			if (!isCollidingCheck(_other)) break;
			transform().lock()->setPos(glm::vec3(currentPos.x, currentPos.y + amount, currentPos.z));
			amount += step;

			if (amount > 100) break; // stop infinate looping
		}
	}

	/*
   *  isCollidingCheck() compared the positon of both the box colliders and their radius's to see if they overlap
   */
	bool BoxCollider::isCollidingCheck(std::shared_ptr<BoxCollider> _other)
	{
		glm::vec3 a = transform().lock()->getPos(); // This objects position
		glm::vec3 b = _other->transform().lock()->getPos(); // Colliding objects position
		glm::vec3 ahs = m_size / 2.0f; // This objects radius
		glm::vec3 bhs = _other->m_size / 2.0f; // Colliding objects radius

		// X INTERSECTION
		if (a.x > b.x)
		{
			if (b.x + bhs.x < a.x - ahs.x)
			{
				return false;
			}
		}
		else
		{
			if (a.x + ahs.x < b.x - bhs.x)
			{
				return false;
			}
		}

		// Y INTERSECTION
		if (a.y > b.y)
		{
			if (b.y + bhs.y < a.y - ahs.y)
			{
				return false;
			}
		}
		else
		{
			if (a.y + ahs.y < b.y - bhs.y)
			{
				return false;
			}
		}

		// Z INTERSECTION
		if (a.z > b.z)
		{
			if (b.z + bhs.z < a.z - ahs.z)
			{
				return false;
			}
		}
		else
		{
			if (a.z + ahs.z < b.z - bhs.z)
			{
				return false;
			}
		}

		return true;
	}

}