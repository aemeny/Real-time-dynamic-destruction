#include "GraphicsModel.h"
#include <iostream>
namespace Renderer
{
	/* loads model from given file location */
	Model::Model(std::string _fileName) : m_vertices(0)
	{
		m_modelId = buLoadModel(_fileName, m_vertices, m_faces);
	}

	void Model::updateModel()
	{
		buUpdateModel(m_modelId, m_faces);
	}
}


