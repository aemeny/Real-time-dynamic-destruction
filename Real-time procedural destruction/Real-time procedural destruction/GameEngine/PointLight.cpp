#include "PointLight.h"
#include "ModelRenderer.h"
#include <iostream>

namespace GameEngine
{
	/*
	* initialize function called from component, sets default file path
	*/
	void PointLight::initialize()
	{
		m_position = glm::vec3(0);
		m_filepath = "../Shaders/Light/fragmentShader.txt"; // Default path for engine
	}

	/*
	* sets new light postion by creating a new stream to be added to the fragment shader in renderer
	* loops through all the model loaders to update the shaders
	*/
	void PointLight::setLightPos(glm::vec3 _pos)
	{
        std::string newPosString = "vec3 lightPos = vec3(" + std::to_string(_pos.x) + ", " + std::to_string(_pos.y)
                                     + ", " + std::to_string(_pos.z) + ");";
 
		//Find all model loaders
		std::vector<std::shared_ptr<ModelRenderer> > modelRenderers;
		core().lock()->find<ModelRenderer>(modelRenderers);
		for(std::shared_ptr<ModelRenderer> modelRenderer : modelRenderers)
		{
			modelRenderer->setShaderLightPos(newPosString);
		}
	}

}
