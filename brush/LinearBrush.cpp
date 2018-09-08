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
    : Brush(color, radius)
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
    paintHere(x, y, canvas);
}

void LinearBrush::brushDragged(int x, int y, Canvas2D *canvas) {
    paintHere(x, y, canvas);
}

void LinearBrush::brushUp(int x, int y, Canvas2D *canvas) {

}

void LinearBrush::paintHere(int x, int y, Canvas2D *canvas) {
    BGRA *pix = canvas->data();

    int w = canvas->width();
    int h = canvas->height();
    int r = getRadius();

    float mask, srcA, dstA, outA;

    for (int row = glm::max(0, y - r); row < glm::min(h, y + r + 1); row++) {
        for (int col = glm::max(0, x - r); col < glm::min(w, x + r + 1); col++) {
            int dst = glm::round(glm::sqrt((float)((row - y) * (row - y) + (col - x) * (col - x))));
            if (dst <= getRadius()) {
                mask = m_mask[dst];
                srcA = getAlpha() / 255.;
                dstA = pix[row * w + col].A() / 255.;
                outA = srcA * (1. - mask) + dstA * (1. - srcA) * mask;
                BGRA& p = pix[row * w + col];
//                qDebug("%d, %d before: (%d, %d, %d, %d)", row, col, p.R(), p.G(), p.B(), p.A());
                p = pix[row * w + col] * (1. - mask * srcA) + getBGRA() * mask * srcA;
//                qDebug("%d, %d after: (%d, %d, %d, %d)", row, col, p.R(), p.G(), p.B(), p.A());
            }
        }
    }

    canvas->update();
}
