#pragma once
#include "../Graphics/Graphics.h"
#include "Resource.h"
#include <memory>

namespace GameEngine
{
	/*
	*  Texture struct inheriting from resource, used to load a renderer model and store
	*/
	struct Texture : Resource
	{
	public:
		void onLoad() override; // Init renderer texture

		std::shared_ptr<Renderer::Texture> getTexture() // Getter for renderer texture
		{
			return m_texture;
		}

	private:
		std::shared_ptr<Renderer::Texture> m_texture;
	};
}