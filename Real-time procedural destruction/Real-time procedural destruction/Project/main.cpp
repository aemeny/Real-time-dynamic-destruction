#include "../GameEngine/GameComponents.h"
#include "Curuthers.h"
#include "Road.h"
#include "Crate.h"

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
	std::shared_ptr<Component> cameraEntityCamera = cameraEntity->addComponent<Camera>(core->m_input); // Perspective 3D camera
	std::shared_ptr<Component> entityGUICamera = cameraEntity->addComponent<Camera>(); // Orthographic camera
	std::shared_ptr<Component> entityGUIQuad = cameraEntity->addComponent<QuadRenderer>();
	std::shared_ptr<Component> entitiyAudioListener = cameraEntity->addComponent<AudioListener>();


	// Entity 2 - Curuthers
	std::shared_ptr<Entity> entity = core->addEntity();
	std::shared_ptr<Component> entityTransform = entity->addComponent<Transform>();
	std::weak_ptr<Transform> transformGrab = entity->findComponent<Transform>();
	transformGrab.lock()->setPos(glm::vec3(0.0f, 0.0f, 0.0f));
	transformGrab.lock()->setRot(glm::vec3(0.0f, 0.0f, 0.0f));

	std::shared_ptr<Component> entitiyModelLoader = entity->addComponent<ModelLoader>();
	std::shared_ptr<Component> entitiyCollider = entity->addComponent<BoxCollider>();
	std::weak_ptr<BoxCollider> BoxColliderGrab = entity->findComponent<BoxCollider>();
	BoxColliderGrab.lock()->setStaticObject(false);
	std::shared_ptr<Component> entitiyCuruthers = entity->addComponent<Curuthers>();

	std::shared_ptr<Component> entitiyAudioSource = entity->addComponent<AudioSource>();
	std::weak_ptr<AudioSource> audioSourceGrab = entity->findComponent<AudioSource>();
	audioSourceGrab.lock()->setAudio(core->m_resources->load<Audio>("../Audio/dixie_horn.ogg"));
	audioSourceGrab.lock()->setPosition(transformGrab.lock()->getPos());
	audioSourceGrab.lock()->startAudio();


	// Entity 3 - Point light
	std::shared_ptr<Entity> entityLight = core->addEntity();
	std::shared_ptr<Component> entityLightPointLight = entityLight->addComponent<PointLight>();
	std::weak_ptr<PointLight> entityPointLightGrab = entityLight->findComponent<PointLight>();
	entityPointLightGrab.lock()->setFilePath("../Shaders/Light/fragmentShader.txt");
	entityPointLightGrab.lock()->setLightPos(glm::vec3(0.0f, 10.0f, 0.0f));


	// Entity 4 - Road
	std::shared_ptr<Entity> entityRoad = core->addEntity();
	std::shared_ptr<Component> entityRoadTransform = entityRoad->addComponent<Transform>();
	transformGrab = entityRoad->findComponent<Transform>();
	transformGrab.lock()->setPos(glm::vec3(0.5f, -3.0f, 12.5f));
	transformGrab.lock()->setRot(glm::vec3(0.0f, 90.0f, 0.0f));
	transformGrab.lock()->setScale(glm::vec3(2.0f, 2.0f, 0.75f));

	std::shared_ptr<Component> entityRoadModelLoader = entityRoad->addComponent<ModelLoader>();
	std::shared_ptr<Component> entityRoadRoad = entityRoad->addComponent<Road>();
	std::shared_ptr<Component> entityRoadCollider = entityRoad->addComponent<BoxCollider>();
	std::weak_ptr<BoxCollider> entityRoadColliderGrab = entityRoad->findComponent<BoxCollider>();
	entityRoadColliderGrab.lock()->setSize(glm::vec3(5.0f, 1.0f, 30.0f));


	// Entity 5 Crate
	std::shared_ptr<Entity> entityCrate = core->addEntity();
	std::shared_ptr<Component> entityCrateTransform = entityCrate->addComponent<Transform>();
	transformGrab = entityCrate->findComponent<Transform>();
	transformGrab.lock()->setPos(glm::vec3(0.5f, -2.0f, 15.0f));

	std::shared_ptr<Component> entityCrateModelLoader = entityCrate->addComponent<ModelLoader>();
	std::shared_ptr<Component> entityCrateCollider = entityCrate->addComponent<BoxCollider>();
	std::weak_ptr<BoxCollider> entityCrateColliderGrab = entityCrate->findComponent<BoxCollider>();
	entityCrateColliderGrab.lock()->setSize(glm::vec3(1.0f, 0.3f, 1.0f));

	std::shared_ptr<Component> entityCrateCrate= entityCrate->addComponent<Crate>();


	// Entity 6 - GUI Button                               
	std::shared_ptr<Entity> entityGUI = core->addEntity();
	std::shared_ptr<Component> entityGUITransform = entityGUI->addComponent<Transform>();
	transformGrab = entityGUI->findComponent<Transform>();
	transformGrab.lock()->setPos(glm::vec3{ core->m_nativeWindow->m_windowWidth / 2, core->m_nativeWindow->m_windowHeight / 2, 0.0f });
	transformGrab.lock()->setRot(glm::vec3{ 180.0f, 0.0f, 0.0f });
	transformGrab.lock()->setScale(glm::vec3{ (core->m_nativeWindow->m_windowWidth / 2.0f),
											  (core->m_nativeWindow->m_windowHeight / 2.0f),
											  0.0f });

	std::shared_ptr<Component> entityGUIGUI = entityGUI->addComponent<GUI>();
	std::weak_ptr<GUI> entityGUIGrab = entityGUI->findComponent<GUI>();
	entityGUIGrab.lock()->setTexture("../Samples/Quit/Quit.png");
	entityGUIGrab.lock()->setClickable(true);


	// After entity set up run core main loop
	core->run();

	return 0;
}

