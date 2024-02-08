#pragma once
#include "../Graphics/Graphics.h"
#include "Resource.h"
#include <memory>

namespace GameEngine 
{
	/*
	*  Model struct inheriting from resource, used to load a renderer model and store
	*/
	struct Model : Resource
	{
	public:
		void onLoad() override; // Load renderer model

		std::shared_ptr<Renderer::Model> getModel() // Getter for model
		{
			return m_model;
		}

	private:
		std::shared_ptr<Renderer::Model> m_model;
	};
}