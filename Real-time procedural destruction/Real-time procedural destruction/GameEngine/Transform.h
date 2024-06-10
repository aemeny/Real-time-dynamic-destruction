#pragma once
#include "Component.h"

namespace GameEngine
{
	/*
	*  Transform struct inheriting from component, used to store all transform related variables and update model matrixies
	*/
	struct Transform : Component
	{
	private:
		glm::vec3 m_position;
		glm::vec3 m_rotation;
		glm::vec3 m_scale;

		glm::mat4 m_modelMatrix;
		bool m_dirty;
	public:
		Transform();

		void updateMatrix(); // Update normal model matrix by scale rotation and position
		void updateGUIMatrix(); // Update GUI model matrix by scale rotation and position
		// Setters
		void setRot(glm::vec3 _rot) { m_rotation = _rot; setDirty(true); }
		void setPos(glm::vec3 _pos) { m_position = _pos; setDirty(true);}
		void setScale(glm::vec3 _scale) { m_scale = _scale; setDirty(true);}
		void setDirty(bool _val) { m_dirty = _val; }
		//Getters
		glm::vec3 getRot() { return m_rotation; }
		glm::vec3 getPos() { return m_position; }
		glm::vec3 getScale() { return m_scale; }
		glm::mat4 getModelMatrix() { return m_modelMatrix; }
		bool getDirty() { return m_dirty; }
	};
}