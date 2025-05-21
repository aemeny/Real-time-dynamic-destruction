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
	float FOV = 90.0f;
	entity->addComponent<Camera>(core->m_input, FOV, true); // Perspective 3D camera
	entity->addComponent<Camera>(); // Orthographic camera

	entity->addComponent<QuadRenderer>();
	entity->addComponent<LineRenderer>();


	// Entity 2 - Point light
	entity = core->addEntity();
	entity->addComponent<PointLight>();
	std::weak_ptr<PointLight> entityPointLightGrab = entity->findComponent<PointLight>();
	entityPointLightGrab.lock()->setFilePath("fragmentShader.glsl");
	entityPointLightGrab.lock()->setLightPos(glm::vec3(20.0f, 40.0f, 20.0f));


	// Entity 3 - Desturction Objs
	entity = core->addEntity();
	std::weak_ptr<Transform> transformGrab = entity->m_transform;
	transformGrab.lock()->setPos(glm::vec3(0.0f, 1.40f, 0.0f));
	transformGrab.lock()->setScale(glm::vec3(1.95f, 1.3f, 0.16f));

	entity->addComponent<ModelRenderer>();
	entity->addComponent<DestructionObjs>();
	entity->addComponent<DestructionHandler>();
	std::weak_ptr<DestructionHandler> destructionHandlerGrab = entity->findComponent<DestructionHandler>();
	destructionHandlerGrab.lock()->setRandomFromCircle(false);
	destructionHandlerGrab.lock()->setDestructionDiameter(9.0f);
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

