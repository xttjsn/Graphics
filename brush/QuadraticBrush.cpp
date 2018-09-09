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
    : Brush(color, radius), m_painter(std::make_unique<SimpleBrushPainter>())
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
    m_mask.reserve(r+1);
    std::fill(m_mask.begin(), m_mask.end(), 0.f);
    for (int i = 0; i < getRadius()+1; i++) {
        m_mask[i] = 1.f - static_cast<float>(i) / static_cast<float>(r);
        m_mask[i] *= m_mask[i];
    }
}

void QuadraticBrush::brushDown(int x, int y, Canvas2D *canvas) {
    m_painter->paint(m_mask, getBGRA(), getRadius(), x, y, canvas);
}

void QuadraticBrush::brushDragged(int x, int y, Canvas2D *canvas) {
    m_painter->paint(m_mask, getBGRA(), getRadius(), x, y, canvas);
}

void QuadraticBrush::brushUp(int x, int y, Canvas2D *canvas) {

}

