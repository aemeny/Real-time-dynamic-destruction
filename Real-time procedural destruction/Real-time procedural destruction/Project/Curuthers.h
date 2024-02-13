#pragma once
#include "../GameEngine/ModelLoader.h"
#include "../GameEngine/Transform.h"

/*
*  Curuthers struct inheriting from component, holds init, deconstructor and tick function
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
};