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
    : Brush(color, radius)
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
    for (int i = 0; i < this->getRadius(); i++) {
        m_mask[i] = 1.;    // All mask values are 1.
    }
}

void ConstantBrush::brushDown(int x, int y, Canvas2D *canvas) {
    qDebug("Brush down at %d, %d.", x, y);
    paintHere(x, y, canvas);
}

void ConstantBrush::brushDragged(int x, int y, Canvas2D *canvas) {
    qDebug("Brush dragged at %d, %d.", x, y);
    paintHere(x, y, canvas);
}

void ConstantBrush::brushUp(int x, int y, Canvas2D *canvas) {
    qDebug("Brush up at %d, %d.", x, y);
}

void ConstantBrush::paintHere(int x, int y, Canvas2D *canvas) {
    BGRA *pix = canvas->data();

    int w = canvas->width();
    int h = canvas->height();
    int r = getRadius();

    float mask, srcA, dstA, outA;

    for (int row = glm::max(0, y - r); row < glm::min(h, y + r + 1); row++) {
        for (int col = glm::max(0, x - r); col < glm::min(w, x + r + 1); col++) {
            int dst = glm::round(glm::sqrt((float)((row - y) * (row - y) + (col - x) * (col - x))));
            if (dst < getRadius()) {
                mask = m_mask[dst];
                srcA = getAlpha() / 255.;
                dstA = pix[row * w + col].A() / 255.;
                outA = srcA * (1. - mask) + dstA * (1. - srcA) * mask;
                pix[row * w + col] = pix[row * w + col] * (1. - mask * srcA) + getBGRA() * mask * srcA;
            }
        }
    }

    canvas->update();
}
