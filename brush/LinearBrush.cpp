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
    : Brush(color, radius), m_painter(std::make_unique<SimpleBrushPainter>())
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
    m_mask.reserve(r+1);
    std::fill(m_mask.begin(), m_mask.end(), 0.f);
    for (int i = 0; i < getRadius()+1; i++) {
        m_mask[i] = 1. - static_cast<float>(i) / static_cast<float>(r);
    }
}

void LinearBrush::brushDown(int x, int y, Canvas2D *canvas) {
    m_painter->paint(m_mask, getBGRA(), getRadius(), x, y, canvas);
}

void LinearBrush::brushDragged(int x, int y, Canvas2D *canvas) {
    m_painter->paint(m_mask, getBGRA(), getRadius(), x, y, canvas);
}

void LinearBrush::brushUp(int x, int y, Canvas2D *canvas) {

}
