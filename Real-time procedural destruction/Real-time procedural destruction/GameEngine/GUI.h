#pragma once
#include "Component.h"
#include "Texture.h"
#include "QuadRenderer.h"

namespace GameEngine
{
	/*
	*  Exception struct inheriting from Component, used to display and hold GUI data
	*/
	struct GUI : Component
	{
		void initialize() override;
		void onDisplay() override; // Displays GUI is m_shown is true
		void onTick() override;
		void checkClickGUI(); // Checks if both the mouse input has happened and intersection with the button for a click
		bool rectIntersect(); // Checks mouse position compared to GUI postion for intersection

		std::shared_ptr<Renderer::Texture> getTexture() { return m_texture; }
		void setTexture(std::string _texturePath, bool modifiableTexture = false)
		{
			m_texture = core().lock()->m_resources->load<Texture>(modifiableTexture, "../Samples/Textures/GUI/" + _texturePath)->getTexture();
		}

		void setClickable(bool _clickable) { m_clickable = _clickable; } // Setter for if the GUI is a button
		void setShown(bool _shown) { m_shown = _shown; } // Setter for if the GUI can be seen

	private:
		bool m_clickable;
		bool m_shown;
		// GUI texture
		std::shared_ptr<Renderer::Texture> m_texture;

		// GUI renderer
		std::weak_ptr<QuadRenderer> m_quadRenderer;
	};
}

