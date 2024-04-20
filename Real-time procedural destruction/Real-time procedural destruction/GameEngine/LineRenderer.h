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
		void onDisplay() override;
		void renderLine(); 

		void addLine(std::weak_ptr<Renderer::Vbo> _vbo, glm::vec3 _start, glm::vec3 _end);
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