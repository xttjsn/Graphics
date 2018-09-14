/**
 * @file   ConstantBrush.cpp
 *
 * Implementation of a brush with a constant mask distribution.
 *
 * You should fill this file in while completing the Brush assignment.
 */

#include "ConstantBrush.h"
#include "qlogging.h"
#include "glm.hpp"
#include "Canvas2D.h"

ConstantBrush::ConstantBrush(BGRA color, int radius)
    : Brush(color, radius), m_painter(std::make_unique<SimpleBrushPainter>())
{
    // @TODO: [BRUSH] You'll probably want to set up the mask right away.
    makeMask();
}


ConstantBrush::~ConstantBrush()
{
}

void ConstantBrush::makeMask() {
    // @TODO: [BRUSH] Set up the mask for your Constant brush here...

    // m_mask[i] is the mask value for pixels that are d distances away from the center, where i = round(d)
    // So, for radius = rad, the maximum value for i is (considering a circular brush) rad
    m_mask.reserve(this->getRadius()+1);
    std::fill(m_mask.begin(), m_mask.end(), 0.);       // Use -O3 to speed up std::fill
    for (int i = 0; i < this->getRadius()+1; i++) {
        m_mask[i] = 1.;    // All mask values are 1.
    }
}

void ConstantBrush::brushDown(int x, int y, Canvas2D *canvas) {
    m_painter->paint(m_mask, getBGRA(), getRadius(), x, y, canvas->width(), canvas->height(), canvas->data());
    canvas->update();
}

void ConstantBrush::brushDragged(int x, int y, Canvas2D *canvas) {
//    m_painter->paint(m_mask, getBGRA(), getRadius(), x, y, canvas);
    m_painter->paint(m_mask, getBGRA(), getRadius(), x, y, canvas->width(), canvas->height(), canvas->data());
    canvas->update();
}

void ConstantBrush::brushUp(int x, int y, Canvas2D *canvas) {
}
