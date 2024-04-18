#include "Line.h"
#include "Transform.h"

namespace GameEngine
{
	/*
	* initialize function called from component, sets up line renderer on the camera
	*/
    void Line::initialize()
    {
        //Find quad renderer on camera
        std::vector<std::shared_ptr<LineRenderer>> quadrenderer;
        core().lock()->find<LineRenderer>(quadrenderer);
        m_lineRenderer = quadrenderer[0];
    }


	/*
	* render line to screen by passing itself into the line renderer
	*/
    void Line::onDisplay()
    {
		m_lineRenderer.lock()->renderLine(this);
    }

}