#pragma once
#include <bugl.h>
#include "glm/gtc/matrix_transform.hpp"

namespace Renderer
{

	struct Model
	{
		Model(std::string _fileName);

		/* calls upon the model to be reuploaded to the gpu */
		void updateModel();

		/* returns the models vertices */
		size_t getVertices() const { return m_vertices; };

		/* returns the models id */
		GLuint getModelId() const { return m_modelId; };

		/* returns models tri faces */
		std::vector<bu::Face>* getFaces() { return &m_faces; }
	private:
		GLuint m_modelId;
		size_t m_vertices;
		std::vector<bu::Face> m_faces;
	};

}

