#include "GraphicsModel.h"
#include <iostream>
namespace Renderer
{
	/* loads model from given file location */
	Model::Model(std::string _fileName) : m_vertices(0)
	{
		m_modelId = buLoadModel(_fileName, m_vertices, m_faces);
		if (!m_modelId) {
			std::cerr << "Failed to load model from " << _fileName << std::endl;
			throw std::runtime_error("Failed to load model");
		}
	}

	void Model::updateModel()
	{
		buUpdateModel(m_modelId, m_faces);
	}
}


