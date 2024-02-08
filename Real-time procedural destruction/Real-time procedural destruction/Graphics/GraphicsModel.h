#pragma
#include <bugl.h>
#include "glm/gtc/matrix_transform.hpp"

namespace Renderer
{

	struct Model
	{
		Model(std::string _fileName);

		/* returns the models vertices */
		size_t getVertices() { return m_vertices; };

		/* returns the models id */
		GLuint getModelId() { return m_modelId; };

	private:
		GLuint m_modelId;
		size_t m_vertices;
	};

}

