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
		void setDirty(bool _dirty) { m_dirty = _dirty; }
		int getNumOfVbos() { return m_vbos.size(); }
		std::vector<std::shared_ptr<Vbo>> m_vbos;
	private:
		GLuint m_id;
		bool m_dirty;
	};

}
