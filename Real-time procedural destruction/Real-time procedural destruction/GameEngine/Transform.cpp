#include "Transform.h"
#include <glm/ext.hpp>
#include <glm/glm.hpp>
#include <random>

namespace GameEngine
{
	Transform::Transform()
		: m_position(0.0f, 0.0f, 0.0f), m_rotation(0.0f, 0.0f, 0.0f), m_scale(1.0f),
		m_modelMatrix(glm::mat4(1.0f))
	{
	}
	/*
	* updates model matrix for a nomral 3D model
	*/
	void Transform::updateMatrix()
	{
		m_modelMatrix = glm::scale(glm::mat4(1.0f), m_scale);
		m_modelMatrix = glm::translate(m_modelMatrix, m_position);
		m_modelMatrix = glm::rotate(m_modelMatrix, glm::radians(m_rotation.x), glm::vec3{ 1.0f, 0.0f, 0.0f });
		m_modelMatrix = glm::rotate(m_modelMatrix, glm::radians(m_rotation.y), glm::vec3{ 0.0f, 1.0f, 0.0f });
		m_modelMatrix = glm::rotate(m_modelMatrix, glm::radians(m_rotation.z), glm::vec3{ 0.0f, 0.0f, 1.0f });
	}
	/*
	* updates model matrix for gui
	*/
	void Transform::updateGUIMatrix()
	{
		m_modelMatrix = glm::translate(glm::mat4(1.0f), m_position);
		m_modelMatrix = glm::scale(m_modelMatrix, m_scale);
		m_modelMatrix = glm::rotate(m_modelMatrix, glm::radians(m_rotation.x), glm::vec3{ 1.0f, 0.0f, 0.0f });
		m_modelMatrix = glm::rotate(m_modelMatrix, glm::radians(m_rotation.y), glm::vec3{ 0.0f, 1.0f, 0.0f });
		m_modelMatrix = glm::rotate(m_modelMatrix, glm::radians(m_rotation.z), glm::vec3{ 0.0f, 0.0f, 1.0f });
	}
}