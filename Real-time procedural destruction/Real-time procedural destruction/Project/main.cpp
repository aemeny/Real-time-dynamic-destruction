#include "../GameEngine/GameComponents.h"


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


	// Entity 2 - Point light
	std::shared_ptr<Entity> entityLight = core->addEntity();
	std::shared_ptr<Component> entityLightPointLight = entityLight->addComponent<PointLight>();
	std::weak_ptr<PointLight> entityPointLightGrab = entityLight->findComponent<PointLight>();
	entityPointLightGrab.lock()->setFilePath("../Shaders/Light/fragmentShader.txt");
	entityPointLightGrab.lock()->setLightPos(glm::vec3(0.0f, 10.0f, 0.0f));


	// Entity 3 - GUI Button                               
	std::shared_ptr<Entity> entityGUI = core->addEntity();
	std::shared_ptr<Component> entityGUITransform = entityGUI->addComponent<Transform>();
	std::weak_ptr<Transform> transformGrab = entityGUI->findComponent<Transform>();
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

