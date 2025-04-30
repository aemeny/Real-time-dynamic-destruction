#include "GraphicsTexture.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Renderer
{
	/* using polymorphism to take a texture that is from a file location */
	Texture::Texture(const char* _fileName) : m_width(0), m_height(0), m_textureId(0), m_fromPassedData(false)
	{
		stbi_set_flip_vertically_on_load(true);
		m_data = stbi_load(_fileName, &m_width, &m_height, NULL, 4);
		if (!m_data) throw std::runtime_error("Couldn't Load Data, Check File Name Is Correct?");

		createTexture();
	}

	/* using polymorphism to take in a texture id(from render texture) */
	Texture::Texture(GLuint _textureId) :m_width(1200), m_height(1200), m_textureId(_textureId), m_fromPassedData(false)
	{}

	/* using polymorphism to generate a texture based on data created */
	Texture::Texture(std::vector<float>*_data, int _width, int _height) :
		m_vecData(_data), m_width(_width), m_height(_height), m_textureId(NULL), m_fromPassedData(true)
	{
		createTexture();
	}

	/* texture deconstructor to delete the held texture */
	Texture::~Texture()
	{
		glDeleteTextures(1, &m_textureId);
	}

	/* creatng the texture by creating the id, uploading to the gpu and apply any extra effects e.g.(alpha blending) */
	void Texture::createTexture()
	{
		createId();
		uploadToGPU();

		// Generate Mipmap so the texture can be mapped correctly
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glGenerateMipmap(GL_TEXTURE_2D);

		// Unbind the texture because we are done operating on it
		glBindTexture(GL_TEXTURE_2D, 0);

		// Anisotropic Filtering
		float aniso = 0.0f;
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &aniso);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, aniso);

		//Alpha Blending
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	/* when creating the id make sure there isnt already an id created */
	void Texture::createId()
	{
		if (!m_textureId)
		{
			glGenTextures(1, &m_textureId);
			if (!m_textureId)
				throw std::runtime_error("Failed to allocate Texture Id");
			glBindTexture(GL_TEXTURE_2D, m_textureId);
		}
	}

	/* upload the texture data to the gpu */
	void Texture::uploadToGPU()
	{
		if (!m_fromPassedData)
		{
			// Upload the image data to the bound texture unit in the GPU
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0,
				GL_RGBA, GL_UNSIGNED_BYTE, m_data);

			// Free the loaded data because we now have a copy on the GPU
			free(m_data); 
		}
		else
		{
			// Upload the image data to the bound texture unit in the GPU
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0,
				GL_RGBA, GL_FLOAT, m_vecData->data());
			m_vecData->clear(); // Clears the content of the vector, but does not release memory
			m_vecData->shrink_to_fit(); // Requests the container to reduce its capacity to fit its size
		}
	}

}


