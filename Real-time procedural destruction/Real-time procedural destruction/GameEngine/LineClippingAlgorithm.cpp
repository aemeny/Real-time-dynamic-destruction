#include "LineClippingAlgorithm.h"

namespace GameEngine 
{
    LineClippingAlgorithm::LineClippingAlgorithm(std::weak_ptr<Transform> _transform, ProjectionPlane _plane)
    {
        glm::vec2 center = glm::vec2(0);
        // Convert the center point ot 2D
        switch (_plane)
        {
        case XY:
            center = glm::vec2(_transform.lock()->getPos().x, _transform.lock()->getPos().y);
            break;
        case YZ:
            center = glm::vec2(_transform.lock()->getPos().y, _transform.lock()->getPos().z);
            break;
        case XZ:
            center = glm::vec2(_transform.lock()->getPos().x, _transform.lock()->getPos().z);
            break;
        }
        
        // Each corner of the rectangle
        m_xmin = center.x - _transform.lock()->getScale().x;
        m_xmax = center.x + _transform.lock()->getScale().x;
        m_ymin = center.y - _transform.lock()->getScale().y;
        m_ymax = center.y + _transform.lock()->getScale().y;
    }

    // Cohen-Sutherland clipping algorithm
    Edge LineClippingAlgorithm::CohenSutherland(Edge& _edge)
    {
        float x0 = _edge.m_start.x, y0 = _edge.m_start.y;
        float x1 = _edge.m_end.x, y1 = _edge.m_end.y;
        int outcode0 = ComputeOutCode(x0, y0);
        int outcode1 = ComputeOutCode(x1, y1);
        bool accept = false;

        while (true) {
            if (!(outcode0 | outcode1)) 
            {
                // Accept and exit loop
                accept = true;
                break;
            }
            else if (completelyOutside(outcode0, outcode1)) 
            {
                // Reject and exit loop
                break;
            }
            else 
            {
                // Calculate the line segment to clip from an outside point to an intersection with clip edge
                float x, y;
                int outcodeOut = outcode0 ? outcode0 : outcode1;

                // Find intersection point
                if (outcodeOut & TOP) 
                {
                    x = x0 + (x1 - x0) * (m_ymax - y0) / (y1 - y0);
                    y = m_ymax;
                }
                else if (outcodeOut & BOTTOM) 
                {
                    x = x0 + (x1 - x0) * (m_ymin - y0) / (y1 - y0);
                    y = m_ymin;
                }
                else if (outcodeOut & RIGHT) 
                {
                    y = y0 + (y1 - y0) * (m_xmax - x0) / (x1 - x0);
                    x = m_xmax;
                }
                else if (outcodeOut & LEFT) 
                {
                    y = y0 + (y1 - y0) * (m_xmin - x0) / (x1 - x0);
                    x = m_xmin;
                }

                // Move outside point to intersection point to clip and get ready for next pass
                if (outcodeOut == outcode0) 
                {
                    // Register edge as clipped
                    _edge.m_clipped = 1; // 1 == m_start

                    _edge.m_start.x = x;
                    _edge.m_start.y = y;
                    outcode0 = ComputeOutCode(_edge.m_start.x, _edge.m_start.y);
                }
                else 
                {
                    // Register edge as clipped
                    _edge.m_clipped = 2; // 2 == m_end

                    _edge.m_end.x = x;
                    _edge.m_end.y = y;
                    outcode1 = ComputeOutCode(_edge.m_end.x, _edge.m_end.y);
                }
            }
        }
        if (accept) 
        {
            return _edge; // Return the clipped edge
        }
        else 
        {
            return Edge(glm::vec2(-1), glm::vec2(-1)); // Return an invalid edge if not accepted
        }
    }

    int LineClippingAlgorithm::ComputeOutCode(const float _x, const float _y)
    {
        // Compute the bit code for a point (x, y) using the clip rectangle bounded diagonally by (xmin, ymin), and (xmax, ymax)
        int code = INSIDE;

        if (_x < m_xmin)          // Left of clip window
            code |= LEFT;
        else if (_x > m_xmax)     // Right of clip window
            code |= RIGHT;
        if (_y < m_ymin)          // Below the clip window
            code |= BOTTOM;
        else if (_y > m_ymax)     // Above the clip window
            code |= TOP;

        return code;
    }

    bool LineClippingAlgorithm::completelyOutside(int _outcode1, int _outcode2)
    {
        return (_outcode1 & _outcode2) != 0;
    }
}