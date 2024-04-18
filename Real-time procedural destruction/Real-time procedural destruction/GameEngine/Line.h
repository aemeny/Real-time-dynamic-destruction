#pragma once
#include "Component.h"
#include "LineRenderer.h"

namespace GameEngine
{
	/*
	*  Line struct inheriting from Component, used to display and hold Line data
	*/
	struct Line : Component
	{
		//inherited functions
		void initialize() override;
		void onDisplay() override;

	private:
		// Line renderer
		std::weak_ptr<LineRenderer> m_lineRenderer;
	};
}

