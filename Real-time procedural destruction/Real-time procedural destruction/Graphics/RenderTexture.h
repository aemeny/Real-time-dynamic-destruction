#include <GL/glew.h>
#include <stdexcept>

namespace Renderer
{

	struct RenderTexture
	{
		RenderTexture(int _width, int _height);
		~RenderTexture();

		void bind();
		void unbind();
		GLuint getTexture();

	private:
		GLuint m_fboId;
		GLuint m_texId;
		GLuint m_rboId;
		int m_width;
		int m_height;
		bool m_dirty;
	};

}

