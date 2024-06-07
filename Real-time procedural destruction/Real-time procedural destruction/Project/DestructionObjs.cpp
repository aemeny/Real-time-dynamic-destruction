#include "DestructionObjs.h"

DestructionObjs::~DestructionObjs() {}

DestructionObjs::DestructionObjs() {}

/*
* initialize function called from component, sets up model
*/
void DestructionObjs::initialize()
{
	std::weak_ptr<GameEngine::ModelRenderer> modelRenderer = m_entity.lock()->findComponent<GameEngine::ModelRenderer>();

	modelRenderer.lock()->setModel("Cube/Cube.obj");
	modelRenderer.lock()->setTexture("Floor/Tile_Diffuse.png");

	m_transform = m_entity.lock()->findComponent<GameEngine::Transform>();
}
