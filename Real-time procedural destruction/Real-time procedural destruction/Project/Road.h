#pragma once
#include "../GameEngine/ModelLoader.h"

/*
*  Road struct inheriting from component, hold init and deconstructor, used to store values
*/
struct Road : GameEngine::Component
{
	~Road();
	void initialize() override;
};

