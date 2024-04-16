#include "Camera.h"

/*
*  CODE PRIMARILY USED FROM 3D GRAPHICS PROGRAMMING ASSIGNMENT
*/

namespace GameEngine
{
	/* Camera init used for 3D cam */
	void Camera::initialize(std::weak_ptr<Input> _input, float _FOV)
	{
		m_FOV = _FOV;
		m_windowHeight = core().lock()->m_nativeWindow->m_windowHeight;
		m_windowWidth = core().lock()->m_nativeWindow->m_windowWidth;
		float fixedNP = 0.1f; // Near Plane
		float fixedFP = 400.0f; // Far Plane
		m_projectionMatrix = glm::perspective(glm::radians(m_FOV),
			(float)(m_windowHeight / m_windowWidth),
			fixedNP, fixedFP);

		m_input = _input;
		m_mouseSpeedX = 0.2f; m_mouseSpeedY = 0.2f; 
		m_speed = 0.02f; 
		m_cameraAngleX = 0.0f; m_cameraAngleY = 0.0f;

		m_camRotation = glm::vec3{ 0.0f,0.0f,0.0f };
		m_camPosition = glm::vec3{ 0.0f, 0.0f, -5.0f };
		glm::vec3 initTarget{ -100.0f, 0.0f, 0.0f };
		m_viewingMatrix = glm::lookAt(m_camPosition, initTarget, glm::vec3(0.0f, 1.0f, 0.0f));

		m_orthoCam = false;
	}

	/* Camera init uses for my orphographic camera
		used in displaying gui and render textures */
	void Camera::initialize()
	{
		m_projectionMatrix = glm::ortho(0.0f, 700.0f, 0.0f, 700.0f, 0.0f, 1.0f);
		m_orthoCam = true;
	}

	/* update camera everyframe to get mouse and keyboard input to update position and rotation creating free camera effect */
	void Camera::onTick()
	{
		if (m_orthoCam)
			return;

		glm::vec2 mouseDelta = core().lock()->m_input->getMouseDelta();
		//Getting mouse input from input class and multiplying my set speed then my delta time for a smooth frame by frame feeling
		m_cameraAngleX -= mouseDelta.x * m_mouseSpeedX * getDT();
		m_cameraAngleY -= mouseDelta.y * m_mouseSpeedY * getDT();

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
			m_translation = m_direction * 10.0f * (float)getDT();
			m_camPosition += m_translation;
		}
		else if (core().lock()->m_input->isKey(SDLK_s))
		{
			m_translation = -(m_direction * 10.0f * (float)getDT());
			m_camPosition += m_translation;
		}
		if (core().lock()->m_input->isKey(SDLK_d))
		{
			m_translation = m_right * 10.0f * (float)getDT();
			m_camPosition += m_translation;
		}
		else if (core().lock()->m_input->isKey(SDLK_a))
		{
			m_translation = -(m_right * 10.0f * (float)getDT());
			m_camPosition += m_translation;
		}

		//update viewing matrix with new position and rotation
		m_viewingMatrix = glm::lookAt(m_camPosition, m_camPosition + m_direction, m_up);
	}

	// Creates the initial ray from the camera
	Ray Camera::getRay(glm::vec2 windowPos)
	{
		glm::vec2 output;
		// Map pixel coordinates
		output.x = mapping(windowPos.x, 0, m_windowWidth, -1, 1);
		output.y = mapping(windowPos.y, 0, m_windowHeight, 1, -1);
		// Set planes based on position
		glm::vec4 nearPlane = { output.x, output.y, -1, 1 };
		glm::vec4 farPlane = { output.x, output.y, 1, 1 };

		m_transformationMatrix = glm::inverse(m_viewingMatrix) * glm::inverse(m_projectionMatrix);
		glm::vec4 transformNear = m_transformationMatrix * nearPlane;
		glm::vec4 transformFar = m_transformationMatrix * farPlane;

		glm::vec3 nearProjection;
		nearProjection = transformNear / transformNear.w;

		glm::vec3 farProjection;
		farProjection = transformFar / transformFar.w;

		// Using projections from the screen position, create the direction for the new ray
		glm::vec3 direction = farProjection - nearProjection;

		// Create ray starting at the near projection with the direction into the scene
		Ray ray = Ray(nearProjection, glm::normalize(direction));

		return ray;
	}

	// Maps camera space
	float Camera::mapping(float xold, float xistart, float xiend, float xostart, float xoend)
	{
		return (xold - xistart) * ((xoend - xostart) / (xiend - xistart)) + xostart;
	}

	void Camera::drawDebugRay(Ray _ray)
	{
		glBegin(GL_LINES);
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex3fv(glm::value_ptr(_ray.origin));
		glm::vec3 end = _ray.origin + _ray.direction;
		glVertex3fv(glm::value_ptr(end));
		glEnd();
	}
}
