#include "Entity.h"
#include "Component.h"
#include "Transform.h"

namespace GameEngine
{
	/// Constructor making a transform object
	Entity::Entity()
	{
		m_transform = addComponent<Transform>();
	}

	/// Getter function.
	bool Entity::getAlive()
	{
		return m_alive;
	}

	/// Loops each component in the entity and calls display
	void Entity::display()
	{
		for (size_t ci = 0; ci < m_components.size(); ++ci)
		{
			m_components.at(ci)->display();
		}
	}

	/// Loops each component in the entity and calls tick
	void Entity::tick()
	{
		for (size_t ci = 0; ci < m_components.size(); ++ci)
		{
			m_components.at(ci)->tick();
		}
	}

	/// Loops each gui component in the entity and calls gui render
	void Entity::gui()
	{
		for (size_t ci = 0; ci < m_components.size(); ++ci)
		{
			m_components.at(ci)->gui();
		}
	}

	/// Loops each component in the entity and kills them
	void Entity::kill()
	{
		if (!m_alive) return;
		m_alive = false;

		for (size_t ci = 0; ci < m_components.size(); ++ci)
		{
			//m_components.at(ci)->kill();
		}
	}

}