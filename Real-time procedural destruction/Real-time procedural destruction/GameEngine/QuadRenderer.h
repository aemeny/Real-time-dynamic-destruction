#pragma once
#include "Camera.h"
#include <GL/glew.h>

namespace GameEngine
{
	struct GUI;

	/*
	*  QuadRenderer struct inheriting from Component, used load a given quad with a texture
	*/
	struct QuadRenderer : Component
	{
	public:
		~QuadRenderer();

		void initialize() override;
		void renderQuad(GUI* _gui); // render a quad based on GUI location an texture

	private:
		std::shared_ptr<Renderer::Vao> m_quad;
		std::shared_ptr<Renderer::Shader> m_shader;

		std::weak_ptr<Camera> m_camera;
	};
}