/**
 * @file   QuadraticBrush.cpp
 *
 * Implementation of a brush with a quadratic mask distribution.
 *
 * You should fill this file in while completing the Brush assignment.
 */

#include "QuadraticBrush.h"

#include <math.h>
#include "glm.hpp"

QuadraticBrush::QuadraticBrush(BGRA color, int radius)
    : ConstantBrush(color, radius)
{
    // @TODO: [BRUSH] You'll probably want to set up the mask right away.
    makeMask();
}

QuadraticBrush::~QuadraticBrush()
{
}

void QuadraticBrush::makeMask() {
    // @TODO: [BRUSH] Set up the mask for your Quadratic brush here...
    int r = getRadius();
    m_mask.resize(r + 1);
    for (int i = 0; i < getRadius()+1; i++) {
        m_mask[i] = 1.f - static_cast<float>(i) / static_cast<float>(r);
        m_mask[i] *= m_mask[i];
    }
}
