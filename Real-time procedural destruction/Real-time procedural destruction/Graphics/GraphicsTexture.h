#pragma
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <stdexcept>
#include <vector>

namespace Renderer
{

	struct Texture
	{
		Texture(const char* _fileName);
		Texture(GLuint _textureId);
		Texture(std::vector<float>* _data, int _width, int _height);
		~Texture();

		GLuint getId() { return m_textureId; };

	private:

		void createTexture();
		void createId();
		void uploadToGPU();

		GLuint m_textureId;
		int m_width;
		int m_height;
		bool m_fromPassedData;

		std::vector<float>* m_vecData;
		unsigned char* m_data;
	};

}
