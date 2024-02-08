#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include <stdexcept>
namespace Renderer
{

	struct Vbo
	{
		Vbo();
		void add(const glm::vec3& _value);
		void add(const glm::vec2& _value);
		GLuint getId();
		int getComponents() { if (!m_components) throw std::runtime_error("m_components not yet set"); return m_components; };

	private:
		GLuint m_id;
		std::vector<float> m_data;
		bool m_dirty;
		int m_components;
	};

}