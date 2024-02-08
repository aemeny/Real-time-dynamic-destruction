#pragma once
#include "glm/gtc/matrix_transform.hpp"
#include "Input.h"
#include "Component.h"

namespace GameEngine 
{
	/*
	*  Camera struct inheriting from component, has a constructor for both orthographic and perspective cameras, has free
	*  cam movement built in to tick for the game
	*/
	struct Camera : Component
	{
		void initialize(std::weak_ptr<Input> _input) override; // Perspective init
		void initialize() override; // Orthographic init
		void onTick() override; // Updates camera position and rotation based off inputs

		glm::mat4 getView() { return m_viewingMatrix; }; // Getter for viewing matrix
		glm::mat4 getProj() { return m_projectionMatrix; }; // Getter for projection matrix
		void setPos(glm::vec3 _pos) { m_camPosition = _pos; } // Setter for camera position
		void setRot(glm::vec3 _rot) { m_camRotation = _rot; } // Setter for rotation
		float getRot() { return m_cameraAngleX; } // Getter for camera rotation

		bool m_orthoCam; // check if orthographic camera
	private:

		// Input pointer reference
		std::weak_ptr<Input> m_input;

		// Matrixies
		glm::mat4 m_projectionMatrix;
		glm::mat4 m_viewingMatrix;

		// Transform variables
		glm::vec3 m_camPosition;
		glm::vec3 m_camRotation;

		// Camera movement based variables
		glm::vec3 m_upVector;
		glm::vec3 m_translation;

		glm::vec3 m_direction;
		glm::vec3 m_right;
		glm::vec3 m_up;

		float m_cameraAngleY;
		float m_cameraAngleX;
		float m_speed;
		float m_mouseSpeedX;
		float m_mouseSpeedY;

	};

}