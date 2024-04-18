#include "Component.h"

namespace GameEngine
{
	/*
   * Virtual functions
   */
	void Component::initialize(){}

	void Component::initialize(std::weak_ptr<Input> _input, float _FOV){}

	void Component::onTick(){}

	void Component::onGUI(){}

	void Component::onDisplay(){}

	void Component::tick()
	{
		onTick();
	}

	void Component::gui()
	{
		onGUI();
	}

	void Component::display()
	{
		onDisplay();
	}

}