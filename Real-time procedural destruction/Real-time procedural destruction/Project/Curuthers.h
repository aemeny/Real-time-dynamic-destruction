#pragma once
#include "../GameEngine/ModelLoader.h"
#include "../GameEngine/Transform.h"
#include "../GameEngine/Camera.h"
#include "../GameEngine/BoxCollider.h"

/*
*  Curuthers struct inheriting from component, holds init, deconstructor and tick function
*  hold box collider, camera and transform to reference during tick, has set physics variables
*/
struct Curuthers : GameEngine::Component
{
	~Curuthers();
	Curuthers();
	void initialize() override;
	void onTick() override;
private:
	// Object references
	std::weak_ptr<GameEngine::Transform> m_transform;
	std::weak_ptr<GameEngine::Camera> m_camera;
	std::weak_ptr<GameEngine::BoxCollider> m_boxCollider;

	// Physics variables
	bool m_jumping;
	float m_gravity;
	float m_speed;
	float m_currentVelocity;
	float m_jumpForce;
};