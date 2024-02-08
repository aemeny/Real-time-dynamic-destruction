#include <string>
#include "Vbo.h"
#include <memory>

namespace Renderer
{

	struct Vao
	{
		Vao();
		GLuint getId();
		void addVbo(std::shared_ptr<Vbo> _vbo);
	private:
		GLuint m_id;
		std::vector<std::shared_ptr<Vbo>> m_vbos;
		bool m_dirty;
	};

}
