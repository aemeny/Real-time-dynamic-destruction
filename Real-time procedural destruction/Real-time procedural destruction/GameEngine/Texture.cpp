#include "Texture.h"

namespace GameEngine
{
	/*
   * loads renderer texture constructor
   */
	void Texture::onLoad()
	{
		m_texture = std::make_shared<Renderer::Texture>(getPath().c_str());
	}
}