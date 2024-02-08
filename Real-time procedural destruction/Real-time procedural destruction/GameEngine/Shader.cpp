#include "Shader.h"

namespace GameEngine
{
	/*
   * loads renderer shader constructor
   */
	void Shader::onLoad()
	{
		m_shader = std::make_shared<Renderer::Shader>(getPath(), getPath2());
	}
}