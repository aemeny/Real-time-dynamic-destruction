#include "Camera.h"

/*
*  CODE PRIMARILY USED FROM 3D GRAPHICS PROGRAMMING ASSIGNMENT
*/

namespace GameEngine
{
	/* Camera init used for 3D cam */
	void Camera::initialize(std::weak_ptr<Input> _input, float _FOV, std::weak_ptr<Camera> _self, bool _mainCamera)
	{
		m_FOV = _FOV;
		m_windowHeight = core().lock()->m_nativeWindow->m_windowHeight;
		m_windowWidth = core().lock()->m_nativeWindow->m_windowWidth;
		float fixedNP = 0.01f; // Near Plane
		float fixedFP = 1000.0f; // Far Plane
		m_projectionMatrix = glm::perspective(glm::radians(m_FOV),
			(float)m_windowWidth / (float)m_windowHeight,
			fixedNP, fixedFP);

		m_input = _input;
		m_mouseSpeedX = 0.003f; m_mouseSpeedY = 0.003f; 
		m_speed = 0.02f; 
		m_cameraAngleX = glm::radians(180.0f); m_cameraAngleY = 0.0f;

		m_camRotation = glm::vec3{ 0.0f, 0.0f, 0.0f };
		m_camPosition = glm::vec3{ 0.0f, 1.41f, 1.75f };
		glm::vec3 initTarget{ 0.0f, 0.0f, 0.0f };
		m_viewingMatrix = glm::lookAt(m_camPosition, initTarget, glm::vec3(0.0f, 1.0f, 0.0f));

		m_orthoCam = false;

		if (_mainCamera)
			core().lock()->setMainCam(_self);
	}

	/* Camera init uses for my orphographic camera
		used in displaying gui and render textures */
	void Camera::initialize()
	{
		m_windowHeight = core().lock()->m_nativeWindow->m_windowHeight;
		m_windowWidth = core().lock()->m_nativeWindow->m_windowWidth;
		m_projectionMatrix = glm::ortho(0.0f, (float)m_windowWidth, 0.0f, (float)m_windowHeight, -1.0f, 1.0f);
		m_orthoCam = true;
	}

	/* update camera everyframe to get mouse and keyboard input to update position and rotation creating free camera effect */
	void Camera::onTick()
	{
		if (m_orthoCam)
			return;

		glm::vec2 mouseDelta = core().lock()->m_input->getMouseDelta();
		//Getting mouse input from input class and multiplying my set speed then my delta time for a smooth frame by frame feeling
		m_cameraAngleX -= mouseDelta.x * m_mouseSpeedX;
		m_cameraAngleY -= mouseDelta.y * m_mouseSpeedY;

		//Lock Camera turning in the y so the camera doesnt flip upside down
		if (m_cameraAngleY < -1)
			m_cameraAngleY = -1;
		else if (m_cameraAngleY > 0.7f)
			m_cameraAngleY = 0.7f;

		//Finding forwards, right and up direction using trig and cross product
		m_direction = glm::vec3(cos(m_cameraAngleY) * sin(m_cameraAngleX), sin(m_cameraAngleY), cos(m_cameraAngleY) * cos(m_cameraAngleX));
		m_right = glm::vec3(sin(m_cameraAngleX - 3.14f / 2.0f), 0, cos(m_cameraAngleX - 3.14 / 2.0f));
		m_up = glm::cross(m_right, m_direction);

		//Handle input for translation
		if (core().lock()->m_input->isKey(SDLK_w))
		{
			//translate position in forwards direction by speed multiply then delta time for smooth movement
			m_translation = m_direction * 2.0f * (float)getDT();
			m_camPosition += m_translation;
		}
		else if (core().lock()->m_input->isKey(SDLK_s))
		{
			m_translation = -(m_direction * 2.0f * (float)getDT());
			m_camPosition += m_translation;
		}
		if (core().lock()->m_input->isKey(SDLK_d))
		{
			m_translation = m_right * 2.0f * (float)getDT();
			m_camPosition += m_translation;
		}
		else if (core().lock()->m_input->isKey(SDLK_a))
		{
			m_translation = -(m_right * 2.0f * (float)getDT());
			m_camPosition += m_translation;
		}

		//update viewing matrix with new position and rotation
		m_viewingMatrix = glm::lookAt(m_camPosition, m_camPosition + m_direction, m_up);
	}

	// Creates the initial ray from the camera
	Ray Camera::getRay(glm::vec2 _windowPixelPos)
	{
		glm::vec4 viewport = { 0, 0, m_windowWidth, m_windowHeight };
		glm::vec3 near = glm::unProject(glm::vec3(_windowPixelPos.x, m_windowHeight - _windowPixelPos.y, -1), m_viewingMatrix, m_projectionMatrix, viewport);

		Ray ray = Ray(near, m_direction);

		return ray;
	}
}
