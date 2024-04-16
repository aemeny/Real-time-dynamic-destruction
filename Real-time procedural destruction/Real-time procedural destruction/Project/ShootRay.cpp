#include "ShootRay.h"
#include "..\Physics\RayTracer.h"

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
}

void ShootRay::onTick()
{
	if (core().lock()->m_input->isMouseClicked())
	{
		createRay();
	}

	for (int i = 0; i < m_generatedRays.size(); i++)
	{
		m_camera.lock()->drawDebugRay(m_generatedRays[i]);
	}

}

void ShootRay::createRay()
{
	GameEngine::Ray ray = m_camera.lock()->getRay(m_screenPos);
	m_generatedRays.push_back(ray);
	core().lock()->m_rayTracer->traceRay(ray);
}

