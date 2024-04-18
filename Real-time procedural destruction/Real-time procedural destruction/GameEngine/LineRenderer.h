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
		void renderLine(Line* _line); 

	private:
		std::shared_ptr<Renderer::Vao> m_line;
		std::shared_ptr<Renderer::Shader> m_shader;

		std::weak_ptr<Camera> m_camera;
	};
}