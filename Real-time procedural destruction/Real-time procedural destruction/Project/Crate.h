#pragma once
#include "../GameEngine/ModelLoader.h"
#include "../GameEngine/Transform.h"
#include "../GameEngine/BoxCollider.h"

/*
*  Road struct inheriting from component, holds init, deconstructor and tick function
* hold box collider and transform to reference during tick, has set speed variable
*/
struct Crate : GameEngine::Component
{
	~Crate();
	void initialize() override;
	void onTick() override;

private:
	std::weak_ptr<GameEngine::Transform> m_transform; 
	std::weak_ptr<GameEngine::BoxCollider> m_boxCollider;

	float m_speed;
};


