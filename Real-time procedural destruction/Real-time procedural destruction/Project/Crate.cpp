#include "Crate.h"
#include <demo/Curuthers.h>
#include <GameEngine/GUI.h>

Crate::~Crate() {}

/*
* initialize function called from component, sets up model loader, grabs the entitys transform and box collider components
*/
void Crate::initialize()
{
	std::weak_ptr<GameEngine::ModelLoader> modelLoader = m_entity.lock()->findComponent<GameEngine::ModelLoader>();

	modelLoader.lock()->setModel("../Samples/Crate/Crate.obj");
	modelLoader.lock()->setTexture("../Samples/Crate/Crate.png");

	m_transform = m_entity.lock()->findComponent<GameEngine::Transform>();
	m_boxCollider = m_entity.lock()->findComponent<GameEngine::BoxCollider>();

	m_speed = 2.5f;
}

/*
* onTick function used to constantly move the crate towards to player with an increasing speed as it passes.
* on collision with player set gameover to true, unlock the mouse and turn the game over GUI on
*/
void Crate::onTick()
{
	m_transform.lock()->setPos(glm::vec3(m_transform.lock()->getPos().x, 
		m_transform.lock()->getPos().y, m_transform.lock()->getPos().z - m_speed * getDT()));

	if (m_transform.lock()->getPos().z < -1.0f)
	{
		m_transform.lock()->setPos(glm::vec3(m_transform.lock()->getPos().x,
			m_transform.lock()->getPos().y, 15.0f));

		m_speed += 1.0f;
	}

	m_boxCollider.lock()->onTick();
	if(m_boxCollider.lock()->isColliding)
	{
		core().lock()->m_gameOver = true;
		core().lock()->m_input->m_lockMouse = false;

		//Find all GUI
		std::vector<std::shared_ptr<GameEngine::GUI> > GUIS;
		core().lock()->find<GameEngine::GUI>(GUIS);
		for each (std::shared_ptr<GameEngine::GUI> GUI in GUIS)
		{
			GUI->setShown(true);
		}
	}
}
