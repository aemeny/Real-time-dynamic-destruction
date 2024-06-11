#include "LineRenderer.h"
#include "Transform.h"

namespace GameEngine
{
	LineRenderer::~LineRenderer()
	{}
	/*
	* initialize function called from component, creats a line (two points)
	*/
	void LineRenderer::initialize()
	{
		m_lines = std::make_shared<Renderer::Vao>();

		m_shader = std::make_shared<Renderer::Shader>("../Shaders/Basic/vertexShader.txt", "../Shaders/Basic/fragmentShader.txt");
	
		//Find all camera to attach main 3D cam
		std::vector<std::shared_ptr<Camera> > cameras;
		core().lock()->find<Camera>(cameras);
		for(std::shared_ptr<Camera> cam : cameras)
		{
			if (cam->m_orthoCam == false)
			{
				m_camera = cam;
			}
		}

		m_linesCreated = 0;
	}

	/*
	* renderes a line based on points given
	*/
	void LineRenderer::renderLine(glm::mat4 _modelMatrix = glm::mat4(1))
	{
		//call shader bind function and pass transform position
		m_shader->bindShader(m_camera.lock()->getProj(), "u_Projection");
		m_shader->bindShader(m_camera.lock()->getView(), "u_Viewing");
		m_shader->bindShader(_modelMatrix, "u_Model");

		//render line
		m_shader->renderLine(m_lines, m_linesCreated);
	}

	/*
	*  Adds a start and end position data to given vbo
	*/
	void LineRenderer::addLine(std::weak_ptr<Renderer::Vbo> _vbo, glm::vec3 _start, glm::vec3 _end)
	{
		for (size_t ei = 0; ei < m_lines->m_vbos.size(); ei++)
		{
			if (m_lines->m_vbos[ei]->getIdentifierID() == _vbo.lock()->getIdentifierID())
			{
				m_lines->m_vbos[ei]->add(_start);
				m_lines->m_vbos[ei]->add(_end);
				m_linesCreated++;
			}
		}
	}

	void LineRenderer::clearLines(std::weak_ptr<Renderer::Vbo> _vbo)
	{
		for (size_t ei = 0; ei < m_lines->m_vbos.size(); ei++)
		{
			if (m_lines->m_vbos[ei]->getIdentifierID() == _vbo.lock()->getIdentifierID())
			{
				m_linesCreated -= m_lines->m_vbos[ei]->getDataSize() / 6; // 6 = 2(Add start + end) * 3(XYZ)
				m_lines->m_vbos[ei]->clearData();
			}
		}
	}

}