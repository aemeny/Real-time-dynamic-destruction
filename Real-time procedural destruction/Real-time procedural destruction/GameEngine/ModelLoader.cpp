#include "ModelLoader.h"
#include "Transform.h"

namespace GameEngine
{
	ModelLoader::~ModelLoader() {}

	/*
	* initialize function called from component, sets up shaders and finds the perspective camera
	*/
	void ModelLoader::initialize()
	{
		//init shader
		m_shader = std::make_shared<Renderer::Shader>("../Shaders/Light/vertexShader.txt", "../Shaders/Light/fragmentShader.txt");
	
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
	}

	/*
	* onDisplay function called from component, updates model matrix and binds the shaders to then render
	*/
	void ModelLoader::onDisplay()
	{
		transform().lock()->updateMatrix();

		m_shader->bindShader(m_camera.lock()->getProj(), "u_Projection");
		m_shader->bindShader(transform().lock()->getModelMatrix(), "u_Model");
		m_shader->bindShader(m_camera.lock()->getView(), "u_Viewing");

		m_shader->render(m_model, m_texture);
	}
}