#pragma once
#include "Component.h"
#include "Texture.h"
#include "Model.h"
#include "Camera.h"
#include <string>

namespace GameEngine
{
	/*
	*  ModelLoader struct inheriting from Component, used to load models and their texture form a file path using a set shader
	*/
	struct ModelRenderer : Component
	{
		~ModelRenderer();

		void initialize() override;
		void onDisplay() override; // Display given model an texture

		void setModel(std::string _modelPath, bool modifiableModel = false) // Set model path and load model
		{
			m_model = core().lock()->m_resources->load<Model>(modifiableModel, "../Samples/Models/" + _modelPath)->getModel();
		}

		std::weak_ptr<Renderer::Model> getModel()
		{
			return m_model;
		}

		void setTexture(std::string _texturePath, bool modifiableTexture = false) // Set texture path and load texture
		{
			m_texture = core().lock()->m_resources->load<Texture>(modifiableTexture, "../Samples/Textures/" + _texturePath)->getTexture();
		}

		void setShaderLightPos(std::string _newLightPos) // Set a new position for a point light in the shader and initliaze
		{
			m_shader->~Shader();
			m_shader = std::make_shared<Renderer::Shader>("../Shaders/Light/vertexShader.txt", 
				"../Shaders/Light/fragmentShader.txt", _newLightPos);
		}

	private:
		// Pointer references 
		std::shared_ptr<Renderer::Model> m_model;
		std::shared_ptr<Renderer::Shader> m_shader;
		std::shared_ptr<Renderer::Texture> m_texture;

		std::weak_ptr<Camera> m_camera;
	};
}