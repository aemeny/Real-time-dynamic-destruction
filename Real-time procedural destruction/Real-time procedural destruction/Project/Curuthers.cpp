#include "Curuthers.h"

Curuthers::~Curuthers(){}

Curuthers::Curuthers(){}

/*
* initialize function called from component, sets up model
*/
void Curuthers::initialize()
{
	std::weak_ptr<GameEngine::ModelRenderer> modelRenderer = m_entity.lock()->findComponent<GameEngine::ModelRenderer>();

	modelRenderer.lock()->setModel("Curuthers/Curuthers.obj");
	modelRenderer.lock()->setTexture("Curuthers/Whiskers_diffuse.png");

	m_transform = m_entity.lock()->findComponent<GameEngine::Transform>();
}

/*
* onTick function called every frame
*/
void Curuthers::onTick()
{
	
}
