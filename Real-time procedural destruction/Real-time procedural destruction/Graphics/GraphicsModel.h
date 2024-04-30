#pragma
#include <bugl.h>
#include "glm/gtc/matrix_transform.hpp"

namespace Renderer
{

	struct Model
	{
		Model(std::string _fileName);

		/* returns the models vertices */
		size_t getVertices() const { return m_vertices; };

		/* returns the models id */
		GLuint getModelId() const { return m_modelId; };

		/* returns models tri faces */
		std::vector<bu::Face> getFaces() { return faces; }

	private:
		GLuint m_modelId;
		size_t m_vertices;
		std::vector<bu::Face> faces;
	};

}

