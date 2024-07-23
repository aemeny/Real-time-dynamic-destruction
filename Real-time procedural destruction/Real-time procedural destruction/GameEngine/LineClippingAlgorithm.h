#pragma once
#include "Voronoi.h"
#include "Transform.h"

namespace GameEngine
{
    enum ProjectionPlane {
        XY, YZ, XZ
    };

    // Outcode constants for Cohen-Sutherland
    const int INSIDE = 0; // 0000
    const int LEFT = 1;   // 0001
    const int RIGHT = 2;  // 0010
    const int BOTTOM = 4; // 0100
    const int TOP = 8;    // 1000

    struct LineClippingAlgorithm
    {
        // Gets rectangle boundaries based on a models scale
        LineClippingAlgorithm(std::weak_ptr<Transform> _transform, ProjectionPlane _plane);

        // Returns a new edge, cut if out of bounds or returns same edge if already accepted
        Edge CohenSutherland(Edge& edge);
    private:
        int ComputeOutCode(const float _x, const float _y);
        bool completelyOutside(int outcode1, int outcode2);

        // Clipping rectangle boundaries
        float m_xmin;
        float m_xmax;
        float m_ymin;
        float m_ymax;
    };
}

