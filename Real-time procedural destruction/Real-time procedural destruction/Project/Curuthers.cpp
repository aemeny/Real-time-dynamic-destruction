#include "Curuthers.h"

Curuthers::~Curuthers(){}

Curuthers::Curuthers() :
	m_jumping(true), m_gravity(-9.81f), m_speed(3.0f), m_jumpForce(15.0f), m_currentVelocity(0)
{}

/*
* initialize function called from component, sets up model loader, grabs the entitys transform, box collider
* and find the 3D perspective camera component
*/
void Curuthers::initialize()
{
	std::weak_ptr<GameEngine::ModelLoader> modelLoader = m_entity.lock()->findComponent<GameEngine::ModelLoader>();

	modelLoader.lock()->setModel("../Samples/Curuthers/curuthers.obj");
	modelLoader.lock()->setTexture("../Samples/Curuthers/Whiskers_diffuse.png");

	m_transform = m_entity.lock()->findComponent<GameEngine::Transform>();

	//Find all camera to attach main 3D cam
	std::vector<std::shared_ptr<GameEngine::Camera> > cameras;
	core().lock()->find<GameEngine::Camera>(cameras);
	for each (std::shared_ptr<GameEngine::Camera> cam in cameras)
	{
		if (cam->m_orthoCam == false)
		{
			m_camera = cam;
		}
	}

	m_boxCollider = m_entity.lock()->findComponent<GameEngine::BoxCollider>();
}

/*
* onTick function used to check if the player is jumping and if so apply gravity physcis else wait for jump input
*/
void Curuthers::onTick()
{
	glm::vec3 currentPos = m_transform.lock()->getPos();

	if (m_jumping)
	{
		m_transform.lock()->setPos(glm::vec3(currentPos.x, currentPos.y + m_currentVelocity * getDT() + 0.5f * (m_gravity)* getDT() * getDT(), currentPos.z));
		m_currentVelocity += m_gravity * getDT();

		//Checking if the player has collided with the floor again and if so stop apply physics and check for key press again
		if (m_boxCollider.lock()->isColliding)
		{
			m_jumping = false;
		}
	}
	if (core().lock()->m_input->isKey(SDLK_SPACE) && m_jumping == false)
	{
		m_jumping = true;
		m_currentVelocity = m_jumpForce;
	}
}
