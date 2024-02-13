#include "Curuthers.h"

Curuthers::~Curuthers(){}

Curuthers::Curuthers(){}

/*
* initialize function called from component, sets up model
*/
void Curuthers::initialize()
{
	std::weak_ptr<GameEngine::ModelLoader> modelLoader = m_entity.lock()->findComponent<GameEngine::ModelLoader>();

	modelLoader.lock()->setModel("Curuthers/Curuthers.obj");
	modelLoader.lock()->setTexture("Curuthers/Whiskers_diffuse.png");

	m_transform = m_entity.lock()->findComponent<GameEngine::Transform>();
}

/*
* onTick function called every frame
*/
void Curuthers::onTick()
{
	
}
