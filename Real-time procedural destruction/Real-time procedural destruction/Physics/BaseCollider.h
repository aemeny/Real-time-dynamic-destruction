#pragma once
#include "..\GameEngine\Component.h"
#include "..\GameEngine\LineRenderer.h"
#include "..\GameEngine\Transform.h"
#include "Ray.h"

namespace GameEngine
{
	// Struct for intersect value information 
	struct intersectionInfo
	{
		glm::vec3 intersectionPos{ 0,0,0 };
		glm::vec3 surfaceNormal{ 0,0,0 };
		float distanceToIntersection = FLT_MAX;
		bool hasIntersected{ false };
		int objIndex = 0;
		bu::Face* collidedFace;
	};

	struct BaseCollider : Component
	{
		// Virtual function to be overrided by children
		virtual intersectionInfo rayIntersect(Ray _ray);
		void setColliderOffset(glm::vec3 _offset) { m_colliderOffset = _offset; }
		virtual void drawOutline(bool _draw);
		void setRenderOutline(bool _set) { m_renderOutline = _set; }

		bool m_renderOutline;
		bool m_lineRendererDirty;
		std::weak_ptr<GameEngine::LineRenderer> m_lineRenderer;
		std::weak_ptr<Renderer::Vbo> m_vbo;
		glm::vec3 m_colliderOffset;
	};

}