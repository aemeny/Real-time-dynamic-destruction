#pragma once
#include "Component.h"

namespace GameEngine
{
	/*
	*  AudioListener struct inheriting from component, hold init and setPos, used to update listener position
	*/
	struct AudioListener : Component
	{
		AudioListener(); 

		void setPos(glm::vec3 _pos); //Set new position and updates the listener through OpenAL

	private:
		glm::vec3 m_position; // Current position of listener
	};
}

