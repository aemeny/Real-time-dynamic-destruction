#include "ShootRay.h"
#include "..\Physics\TraceRay.h"

ShootRay::~ShootRay() {}
ShootRay::ShootRay() {}

void ShootRay::initialize()
{
	//Find all camera to attach main 3D cam
	std::vector<std::shared_ptr<GameEngine::Camera> > cameras;
	core().lock()->find<GameEngine::Camera>(cameras);
	for (std::shared_ptr<GameEngine::Camera> cam : cameras)
	{
		if (cam->m_orthoCam == false)
		{
			m_camera = cam;
		}
	}

	std::vector<std::shared_ptr<GameEngine::LineRenderer> > lineRenderer;
	core().lock()->find<GameEngine::LineRenderer>(lineRenderer);
	m_lineRenderer = lineRenderer[0];

	//m_vbo = m_lineRenderer.lock()->addVbo();
}

void ShootRay::onTick()
{
	if (core().lock()->m_input->isMouseClicked())
	{
		createRay();
	}
}

void ShootRay::onDisplay()
{
	m_lineRenderer.lock()->renderLine();
}

void ShootRay::createRay()
{
	GameEngine::Ray ray = m_camera.lock()->getRay(m_screenPos);
	//m_lineRenderer.lock()->addLine(m_vbo, ray.origin, ray.origin + ray.direction * 100.0f);
	core().lock()->m_traceRay->shootRay(ray);
}

