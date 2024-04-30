#include "../GameEngine/GameComponents.h"
#include "Curuthers.h"
#include "Floor.h"
#include "ShootRay.h"

using namespace GameEngine;

/*
* Main function creating core and applying / setting up all entitys and their components
*/
#undef main
int main()
{
	// Core init
	std::shared_ptr<Core> core = Core::initialize();
	
	// Entity 1 - Camera
	std::shared_ptr<Entity> entity = core->addEntity();
	entity->addComponent<Transform>();
	float FOV = 90.0f;
	entity->addComponent<Camera>(core->m_input, FOV); // Perspective 3D camera
	entity->addComponent<Camera>(); // Orthographic camera
	
	entity->addComponent<QuadRenderer>();
	entity->addComponent<LineRenderer>();
	entity->addComponent<AudioListener>();
		

	// Entity 2 - Point light
	entity = core->addEntity();
	entity->addComponent<PointLight>();
	std::weak_ptr<PointLight> entityPointLightGrab = entity->findComponent<PointLight>();
	entityPointLightGrab.lock()->setFilePath("fragmentShader.txt");
	entityPointLightGrab.lock()->setLightPos(glm::vec3(0.0f, 10.0f, 0.0f));


	// Entity 3 - Curuthers
	entity = core->addEntity();
	entity->addComponent<Transform>();
	entity->addComponent<ModelLoader>();
	entity->addComponent<Curuthers>();

	entity->addComponent<MeshCollider>();
	std::weak_ptr<MeshCollider> meshcolliderGrab = entity->findComponent<MeshCollider>();
	meshcolliderGrab.lock()->setRenderOutline(true);


	// Entity 4 - Floor
	entity = core->addEntity();
	entity->addComponent<Transform>();
	std::weak_ptr<Transform> transformGrab = entity->findComponent<Transform>();
	transformGrab.lock()->setPos(glm::vec3(0.0f, -2.4f, 0.0f));
	transformGrab.lock()->setRot(glm::vec3(0.0f, 0.0f, 0.0f));
	transformGrab.lock()->setScale(glm::vec3(0.05f, 1.0f, 0.05f));

	entity->addComponent<ModelLoader>();
	entity->addComponent<Floor>();

	entity->addComponent<BoxCollider>();
	std::weak_ptr<BoxCollider> boxcolliderGrab = entity->findComponent<BoxCollider>();
	boxcolliderGrab.lock()->setColliderSize(glm::vec3(102.4f, 0.2f, 102.4f));
	boxcolliderGrab.lock()->setRenderOutline(false);



	//Entity 5 - Shootray
	entity = core->addEntity();
	entity->addComponent<ShootRay>();
	std::weak_ptr<ShootRay> entityShootRayGrab = entity->findComponent<ShootRay>();
	entityShootRayGrab.lock()->setPos(glm::vec2(core->m_nativeWindow->m_windowWidth, core->m_nativeWindow->m_windowHeight));
	


	// After entity set up run core main loop
	core->run();

	return 0;
}

