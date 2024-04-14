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
	std::shared_ptr<Entity> cameraEntity = core->addEntity();
	std::shared_ptr<Component> cameraEntityTransform = cameraEntity->addComponent<Transform>();

	float FOV = 90.0f;
	std::shared_ptr<Component> cameraEntityCamera = cameraEntity->addComponent<Camera>(core->m_input, FOV); // Perspective 3D camera
	std::shared_ptr<Component> entityGUICamera = cameraEntity->addComponent<Camera>(); // Orthographic camera
	
	std::shared_ptr<Component> entityGUIQuad = cameraEntity->addComponent<QuadRenderer>();
	std::shared_ptr<Component> entitiyAudioListener = cameraEntity->addComponent<AudioListener>();

	std::shared_ptr<Component> entitiyShootRay = cameraEntity->addComponent<ShootRay>();
	std::weak_ptr<ShootRay> entityShootRayGrab = cameraEntity->findComponent<ShootRay>();
	entityShootRayGrab.lock()->setPos(glm::vec2(core->m_nativeWindow->m_windowWidth, core->m_nativeWindow->m_windowHeight));
		

	// Entity 2 - Point light
	std::shared_ptr<Entity> entityLight = core->addEntity();
	std::shared_ptr<Component> entityLightPointLight = entityLight->addComponent<PointLight>();
	std::weak_ptr<PointLight> entityPointLightGrab = entityLight->findComponent<PointLight>();
	entityPointLightGrab.lock()->setFilePath("fragmentShader.txt");
	entityPointLightGrab.lock()->setLightPos(glm::vec3(0.0f, 10.0f, 0.0f));


	// Entity 3 - Curuthers
	std::shared_ptr<Entity> entity = core->addEntity();
	std::shared_ptr<Component> entityTransform = entity->addComponent<Transform>();
	std::weak_ptr<Transform> transformGrab = entity->findComponent<Transform>();
	transformGrab.lock()->setPos(glm::vec3(0.0f, 0.0f, 0.0f));
	transformGrab.lock()->setRot(glm::vec3(0.0f, 0.0f, 0.0f));

	std::shared_ptr<Component> entitiyModelLoader = entity->addComponent<ModelLoader>();
	std::shared_ptr<Component> entitiyCuruthers = entity->addComponent<Curuthers>();


	// Entity 4 - Floor
	entity = core->addEntity();
	entityTransform = entity->addComponent<Transform>();
	transformGrab = entity->findComponent<Transform>();
	transformGrab.lock()->setPos(glm::vec3(0.0f, -2.4f, 0.0f));
	transformGrab.lock()->setRot(glm::vec3(0.0f, 0.0f, 0.0f));
	transformGrab.lock()->setScale(glm::vec3(0.05f, 1.0f, 0.05f));

	entitiyModelLoader = entity->addComponent<ModelLoader>();
	std::shared_ptr<Component> entitiyFloor = entity->addComponent<Floor>();
	std::shared_ptr<Component> entitiyCollider = entity->addComponent<BoxCollider>();
	std::weak_ptr<BoxCollider> boxcolliderGrab = entity->findComponent<BoxCollider>();
	core->m_rayTracer->addObject(boxcolliderGrab);

	// After entity set up run core main loop
	core->run();

	return 0;
}

