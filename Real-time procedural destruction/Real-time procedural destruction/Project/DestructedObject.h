#pragma once
#include "../GameEngine/Transform.h"

/*
*  Floor struct inheriting from component, holds init, deconstructor and tick function
*/
struct DestructedObject : GameEngine::Component
{
	~DestructedObject();
	DestructedObject();
	void initialize() override;

private:
	// Object references
	std::weak_ptr<GameEngine::Transform> m_transform;
};