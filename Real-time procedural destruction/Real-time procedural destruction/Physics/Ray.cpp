#include "Ray.h"
namespace GameEngine
{
	// Constructor for ray for holding information
	Ray::Ray(glm::vec3 _origin, glm::vec3 _direction)
		: origin(_origin), direction(_direction)
	{}
}