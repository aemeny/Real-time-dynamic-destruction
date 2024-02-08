#include "GUI.h"
#include "Transform.h"

namespace GameEngine
{
	/*
	* initialize function called from component, sets up quad renderer on the camera
	*/
    void GUI::initialize()
    {
        //Find quad renderer on camera
        std::vector<std::shared_ptr<QuadRenderer>> quadrenderer;
        core().lock()->find<QuadRenderer>(quadrenderer);
        m_quadRenderer = quadrenderer[0];
		m_shown = false;
    }


	/*
	* if the gui is shown render to screen and check if clicked if clickable
	*/
    void GUI::onDisplay()
    {
		if (m_shown)
		{
			if (m_clickable)
			{
				checkClickGUI();
			}
			m_quadRenderer.lock()->renderQuad(this);
		}
    }

	void GUI::onTick(){}


	/*
	* If both mouse button is clicked this frame and intersecting with the gui, click
	*/
	void GUI::checkClickGUI()
	{
		if (core().lock()->m_input->isMouseClicked())
		{
			if (rectIntersect())
			{
				core().lock()->stop();
			}
		}
	}


	/*
	* checks the mouse coordinates and compares them to the GUIs to see if its inside the radius
	*/
    bool GUI::rectIntersect()
    {
		glm::vec3 a = transform().lock()->getPos(); // This GUI's position
		glm::vec2 b = glm::vec2(core().lock()->m_input->getMousePos()); // mouses position
		b.y = core().lock()->m_nativeWindow->m_windowHeight - b.y; // flip mouse position to gui space

		float aw = transform().lock()->getScale().x / 2.0f; // This GUI's height
		float ah = transform().lock()->getScale().y / 2.0f; // This GUI's width

		// X INTERSECTION
		if (a.x > b.x)
		{
			if (b.x < a.x - aw)
			{
				return false;
			}
		}
		else
		{
			if (a.x + aw < b.x)
			{
				return false;
			}
		}

		// Y INTERSECTION
		if (a.y > b.y)
		{
			if (b.y < a.y - ah)
			{
				return false;
			}
		}
		else
		{
			if (a.y + ah < b.y)
			{
				return false;
			}
		}

		return true;
    }

}