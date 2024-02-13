#pragma once
#include "Component.h"

namespace GameEngine
{
	/*
	*  PointLight struct inheriting from Component, used to save position and file path for the light shader
	*/
	struct PointLight : Component
	{
		void initialize() override;
		void setFilePath(std::string _path) { m_filepath = "../Shaders/Light/" + _path; } // Setter for the light shader file path
		void setLightPos(glm::vec3 _pos); // Set light position
		glm::vec3 getLightPos() { return m_position; } // Getter for light position

	private:
		glm::vec3 m_position;
		std::string m_filepath;

	};
}
