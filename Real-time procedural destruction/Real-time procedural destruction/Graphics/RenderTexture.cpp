#include "RenderTexture.h"

namespace Renderer
{

	/* init render texture with set width and height */
	RenderTexture::RenderTexture(int _width, int _height)
	{
		m_width = _width;
		m_height = _height;
		m_dirty = true;
	}

	/* On deconstruction make sure frame buffers, textures and render buffers are cleared to stop any leaks */
	RenderTexture::~RenderTexture()
	{
		glDeleteFramebuffers(1, &m_fboId);
		glDeleteTextures(1, &m_texId);
		glDeleteRenderbuffers(1, &m_rboId);
	}

	/* Binds render texture so everything rendered after is bound to this texture */
	void RenderTexture::bind()
	{
		if (m_dirty) // running a dirty flag so a memory leak isnt made when generating the render texture texture each frame
		{
			m_dirty = false;
			//creating the frame buffer
			glGenFramebuffers(1, &m_fboId);
			if (!m_fboId) throw std::exception();
			glBindFramebuffer(GL_FRAMEBUFFER, m_fboId);
			//generating the render textures texture
			glGenTextures(1, &m_texId);
			glBindTexture(GL_TEXTURE_2D, m_texId);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glBindTexture(GL_TEXTURE_2D, 0);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texId, 0);

			glGenRenderbuffers(1, &m_rboId);
			glBindRenderbuffer(GL_RENDERBUFFER, m_rboId);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_width, m_height);
			glBindRenderbuffer(GL_RENDERBUFFER, 0);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_rboId);
		}
		//binding the frame buffer so everything rendered after is put into the render textures frame buffer
		glBindFramebuffer(GL_FRAMEBUFFER, m_fboId);
	}

	/* unbinding the render texture so everything rendered next isnt added to its frame buffer */
	void RenderTexture::unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	/* getting the render textures texture id to create a texture from it */
	GLuint RenderTexture::getTexture()
	{
		glBindTexture(GL_TEXTURE_2D, m_texId);
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);
		return m_texId;
	}

}
