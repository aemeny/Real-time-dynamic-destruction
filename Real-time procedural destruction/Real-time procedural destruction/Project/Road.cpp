#include "Road.h"

Road::~Road() {}

/*
* initialize function called from component, sets up model loader
*/
void Road::initialize()
{
	std::weak_ptr<GameEngine::ModelLoader> modelLoader = m_entity.lock()->findComponent<GameEngine::ModelLoader>();

	modelLoader.lock()->setModel("../Samples/Road/Road.obj");
	modelLoader.lock()->setTexture("../Samples/Road/Road.png");
}
