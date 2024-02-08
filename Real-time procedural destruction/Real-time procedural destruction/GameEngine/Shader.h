#pragma once
#include "../Graphics/Graphics.h"
#include "Resource.h"
#include <memory>

namespace GameEngine
{
	/*
	*  Shader struct inheriting from resource, used to load a renderer model and store
	*/
	struct Shader : Resource
	{
	public:
		void onLoad() override; // Init renderer shader

		std::shared_ptr<Renderer::Shader> getShader() // Getter renderer shader
		{
			return m_shader;
		}

	private:
		std::shared_ptr<Renderer::Shader> m_shader;
	};
}