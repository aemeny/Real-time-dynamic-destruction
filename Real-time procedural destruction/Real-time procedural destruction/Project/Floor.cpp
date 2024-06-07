#include "Floor.h"

Floor::~Floor() {}

Floor::Floor() {}

/*
* initialize function called from component, sets up model
*/
void Floor::initialize()
{
	std::weak_ptr<GameEngine::ModelRenderer> modelRenderer = m_entity.lock()->findComponent<GameEngine::ModelRenderer>();

	modelRenderer.lock()->setModel("Floor/Floor.obj");
	modelRenderer.lock()->setTexture("Floor/Floor.png");

	m_transform = m_entity.lock()->findComponent<GameEngine::Transform>();
}

/*
* onTick function called every frame
*/
void Floor::onTick()
{

}
