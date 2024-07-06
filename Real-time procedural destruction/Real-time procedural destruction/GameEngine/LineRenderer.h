#pragma once
#include "Camera.h"
#include <GL/glew.h>

namespace GameEngine
{
	struct Line;
	/*
	*  LineRenderer struct inheriting from Component
	*/
	struct LineRenderer : Component
	{
	public:
		~LineRenderer();

		void initialize() override;
		void renderLine(glm::mat4 _modelMatrix = glm::mat4(1));

		void addLine(std::weak_ptr<Renderer::Vbo> _vbo, glm::vec3 _start, glm::vec3 _end);
		void clearLines(std::weak_ptr<Renderer::Vbo> _vbo);
		std::weak_ptr<Renderer::Vbo> addVbo()
		{
			std::shared_ptr<Renderer::Vbo> rtn = std::make_shared<Renderer::Vbo>();

			m_lines->addVbo(rtn);

			return rtn;
		}

	private:
		std::shared_ptr<Renderer::Vao> m_lines;
		std::shared_ptr<Renderer::Shader> m_shader;

		std::weak_ptr<Camera> m_camera;

		int m_linesCreated;
	};
}