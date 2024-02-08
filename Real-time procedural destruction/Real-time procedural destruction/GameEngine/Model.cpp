#include "Model.h"

namespace GameEngine
{
	/*
   * loads renderer model constructor
   */
	void Model::onLoad()
	{
		m_model = std::make_shared<Renderer::Model>(getPath());
	}
}