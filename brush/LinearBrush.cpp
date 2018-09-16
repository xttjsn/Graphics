/**
 * @file   LinearBrush.cpp
 *
 * Implementation of a brush with a linear mask distribution.
 *
 * You should fill this file in while completing the Brush assignment.
 */

#include "LinearBrush.h"

#include <math.h>
#include "Canvas2D.h"
#include "glm.hpp"

LinearBrush::LinearBrush(BGRA color, int radius)
    : ConstantBrush(color, radius)
{
    // @TODO: [BRUSH] You'll probably want to set up the mask right away.
    makeMask();
}


LinearBrush::~LinearBrush()
{
}

void LinearBrush::makeMask() {
    // @TODO: [BRUSH] Set up the mask for your Linear brush here...
    // m_mask[i] = 1.0 * (1 - i / r), where i is the distance between the point and the center, and r is the radius
    // So there are r+1 masks
    int r = getRadius();
    m_mask.resize(r + 1);
    for (int i = 0; i < getRadius()+1; i++) {
        m_mask[i] = 1.f - i * (1.f / r);
    }
}
