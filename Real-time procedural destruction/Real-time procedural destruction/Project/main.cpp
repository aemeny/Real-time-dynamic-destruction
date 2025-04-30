#include "../GameEngine/GameComponents.h"
#include "Curuthers.h"
#include "Floor.h"
#include "DestructionObjs.h"
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
	entity->addComponent<Camera>(core->m_input, FOV, true); // Perspective 3D camera
	entity->addComponent<Camera>(); // Orthographic camera

	entity->addComponent<QuadRenderer>();
	entity->addComponent<LineRenderer>();


	//Entity 2 - GUI
	std::shared_ptr<Entity> entityGUI = core->addEntity();
	std::shared_ptr<Component> entityGUITransform = entityGUI->addComponent<Transform>();
	std::weak_ptr<Transform> transformGrab = entityGUI->findComponent<Transform>();
	transformGrab.lock()->setPos(glm::vec3{ core->m_nativeWindow->m_windowWidth / 2, core->m_nativeWindow->m_windowHeight / 2, 0.0f });
	transformGrab.lock()->setRot(glm::vec3{ 0.0f, 0.0f, 0.0f });
	transformGrab.lock()->setScale(glm::vec3{ 8.0f, 8.0f, 0.0f });

	std::shared_ptr<Component> entityGUIGUI = entityGUI->addComponent<GUI>();
	std::weak_ptr<GUI> entityGUIGrab = entityGUI->findComponent<GUI>();
	entityGUIGrab.lock()->setTexture("Crosshair3.png");
	entityGUIGrab.lock()->setShown(true);


	// Entity 3 - Point light
	entity = core->addEntity();
	entity->addComponent<PointLight>();
	std::weak_ptr<PointLight> entityPointLightGrab = entity->findComponent<PointLight>();
	entityPointLightGrab.lock()->setFilePath("fragmentShader.glsl");
	entityPointLightGrab.lock()->setLightPos(glm::vec3(20.0f, 40.0f, 20.0f));

	// Entity 4 - Floor
	/*entity = core->addEntity();
	entity->addComponent<Transform>();
	transformGrab = entity->findComponent<Transform>();
	transformGrab.lock()->setPos(glm::vec3(0.0f, 0.0f, 0.0f));
	transformGrab.lock()->setRot(glm::vec3(0.0f, 0.0f, 0.0f));
	transformGrab.lock()->setScale(glm::vec3(0.006f, 1.0f, 0.006f));

	entity->addComponent<ModelRenderer>();
	entity->addComponent<Floor>();*/

	// Entity 5 - Desturction Objs

	entity = core->addEntity();
	entity->addComponent<Transform>();
	transformGrab = entity->findComponent<Transform>();
	//transformGrab.lock()->setPos(glm::vec3(-22.0f + i * 11.0f, 3.0f + j * 11.0f, -10.0f));
	transformGrab.lock()->setPos(glm::vec3(0.0f, 1.40f, 0.0f));
	transformGrab.lock()->setScale(glm::vec3(1.95f, 1.3f, 0.16f));

	entity->addComponent<ModelRenderer>();
	entity->addComponent<DestructionObjs>();
	entity->addComponent<DestructionHandler>();
	std::weak_ptr<DestructionHandler> destructionHandlerGrab = entity->findComponent<DestructionHandler>();
	destructionHandlerGrab.lock()->setRandomFromCircle(true);
	destructionHandlerGrab.lock()->setDestructionDiameter(4.0f);
	destructionHandlerGrab.lock()->setPointGenerationDepth(50);

	entity->addComponent<MeshCollider>();

	//Entity 6 - Shootray
	entity = core->addEntity();
	entity->addComponent<ShootRay>();
	std::weak_ptr<ShootRay> entityShootRayGrab = entity->findComponent<ShootRay>();
	entityShootRayGrab.lock()->setPos(glm::vec2(core->m_nativeWindow->m_windowWidth, core->m_nativeWindow->m_windowHeight));


	// After entity set up run core main loop
	core->run();

	return 0;
}

