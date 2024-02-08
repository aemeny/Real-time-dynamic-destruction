#pragma
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <stdexcept>


namespace Renderer
{

	struct Texture
	{
		Texture(const char* _fileName);
		Texture(GLuint _textureId);
		~Texture();

		GLuint getId() { return m_textureId; };

	private:

		void createTexture();
		void createId();
		void uploadToGPU();

		int m_width;
		int m_height;
		GLuint m_textureId;
		unsigned char* m_data;
	};

}
