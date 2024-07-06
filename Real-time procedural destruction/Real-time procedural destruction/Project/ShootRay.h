#pragma once
#include "..\GameEngine\Camera.h"
#include "..\GameEngine\LineRenderer.h"

/*
*  ShootRay struct inheriting from component, holds init, deconstructor and tick function
*/
struct ShootRay : GameEngine::Component
{
	~ShootRay();
	ShootRay();
	void initialize() override;
	void onTick() override;
	void onDisplay() override;

	void setPos(glm::vec2 _screenPos) { m_screenPos = _screenPos; }

private:
	glm::vec2 m_screenPos; // Location the ray is being created on screen
	std::weak_ptr<GameEngine::Camera> m_camera; // Main 3D cam
	std::weak_ptr<GameEngine::LineRenderer> m_lineRenderer;
	std::weak_ptr< Renderer::Vbo> m_vbo;

	void createRay(); // Creating and shooting ray
};

