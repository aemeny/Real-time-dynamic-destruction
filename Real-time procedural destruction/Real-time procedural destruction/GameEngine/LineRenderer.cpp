#include "LineRenderer.h"
#include "Transform.h"
#include "Line.h"

namespace GameEngine
{
	LineRenderer::~LineRenderer()
	{}
	/*
	* initialize function called from component, creats a line (two points)
	*/
	void LineRenderer::initialize()
	{
		std::shared_ptr<Renderer::Vbo> pos = std::make_shared<Renderer::Vbo>();
		pos->add(glm::vec3(-0.5f, 0.0f, 0.0f));
		pos->add(glm::vec3(0.5f, 0.0f, 0.0f));

		m_line = std::make_shared<Renderer::Vao>();
		m_line->addVbo(pos);

		m_shader = std::make_shared<Renderer::Shader>("../Shaders/Basic/vertexShader.txt", "../Shaders/Basic/fragmentShader.txt");
	
		//Find all camera to attach main 3D cam
		std::vector<std::shared_ptr<Camera> > cameras;
		core().lock()->find<Camera>(cameras);
		for(std::shared_ptr<Camera> cam : cameras)
		{
			if (cam->m_orthoCam == true)
			{
				m_camera = cam;
			}
		}
	}

	/*
	* renderes a line based on points given
	*/
	void LineRenderer::renderLine(Line* _line)
	{
		//Update Matrix
		_line->transform().lock()->updateGUIMatrix();

		//call shader bind function and pass transform position
		m_shader->bindShader(m_camera.lock()->getProj(), "u_Projection");
		m_shader->bindShader(_line->transform().lock()->getModelMatrix(), "u_Model");

		//render quad
		m_shader->renderQuad(m_line);
	}

}