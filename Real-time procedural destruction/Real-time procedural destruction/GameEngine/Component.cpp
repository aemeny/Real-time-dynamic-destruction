#include "Component.h"

namespace GameEngine
{
	/*
   * Virtual functions
   */
	void Component::initialize(){}

	void Component::initialize(std::weak_ptr<Input> _input, float _FOV, std::weak_ptr<Camera> _self, bool _mainCamera){}

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