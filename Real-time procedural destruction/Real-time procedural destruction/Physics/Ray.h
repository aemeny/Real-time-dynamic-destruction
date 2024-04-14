#pragma once
#include <glm/glm.hpp>

namespace GameEngine
{
	// Ray class, hold info for created rays
	struct Ray
	{
	public:
		glm::vec3 origin; //Origin position x y z
		glm::vec3 direction; // Direction Vector

		Ray(glm::vec3 _origin, glm::vec3 _direction); // Construtor

	};

}