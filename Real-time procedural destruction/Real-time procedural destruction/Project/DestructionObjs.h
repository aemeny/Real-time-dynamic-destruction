#pragma once
#include "../GameEngine/ModelRenderer.h"
#include "../GameEngine/Transform.h"

/*
*  Floor struct inheriting from component, holds init, deconstructor and tick function
*/
struct DestructionObjs : GameEngine::Component
{
	~DestructionObjs();
	DestructionObjs();
	void initialize() override;
private:
	// Object references
	std::weak_ptr<GameEngine::Transform> m_transform;
};

