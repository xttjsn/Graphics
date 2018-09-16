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
    m_mask.resize(getRadius() + 1);
    for (int i = 0; i < getRadius()+1; i++) {
        m_mask[i] = 1.f;    // All mask values are 1.
    }
}


void ConstantBrush::brushDown(int x, int y, Canvas2D *canvas) {
    if (m_fix_alpha_blending)
        paintFixAlpha(x, y, canvas);
    else
        paintNormal(x, y, canvas);
    canvas->update();
}

void ConstantBrush::brushDragged(int x, int y, Canvas2D *canvas) {
    if (m_fix_alpha_blending)
        paintFixAlpha(x, y, canvas);
    else
        paintNormal(x, y, canvas);
    canvas->update();
}

void ConstantBrush::brushUp(int x, int y, Canvas2D *canvas) {
}

void ConstantBrush::paintFixAlpha(int x, int y, Canvas2D* canvas) {
    int r = getRadius(), w = canvas->width(), h = canvas->height();
    float mask, prev_mask, srcA;

    BGRA bgra = getBGRA();
    BGRA* pix = canvas->data();

    for (int row = glm::max(0, y - r); row < glm::min(h, y + r + 1); row++) {
        for (int col = glm::max(0, x - r); col < glm::min(w, x + r + 1); col++) {
            int dst = glm::round(glm::sqrt(static_cast<float>((row - y) * (row - y) + (col - x) * (col - x))));
            if (dst <= r) {
                mask = m_mask[dst];
                srcA = bgra.A() / 255.f;
                prev_mask = m_mask_cache[row * w + col];
                mask = glm::min(mask + prev_mask, 1.f);
                pix[row * w + col] = m_pixel_src[row * w + col] * (1.0f - mask * srcA) + bgra * mask * srcA;
                m_mask_cache[row * w + col] = mask;
            }
        }
    }
}

void ConstantBrush::paintNormal(int x, int y, Canvas2D* canvas) {
    int r = getRadius(), w = canvas->width(), h = canvas->height();
    float mask, srcA;

    BGRA bgra = getBGRA();
    BGRA* pix = canvas->data();

    for (int row = glm::max(0, y - r); row < glm::min(h, y + r + 1); row++) {
        for (int col = glm::max(0, x - r); col < glm::min(w, x + r + 1); col++) {
            int dst = glm::round(glm::sqrt(static_cast<float>((row - y) * (row - y) + (col - x) * (col - x))));
            if (dst <= r) {
                mask = m_mask[dst];
                srcA = bgra.A() / 255.f;
                pix[row * w + col] = pix[row * w + col] * (1.f - mask * srcA) + bgra * mask * srcA;
            } else {
                
            }
        }
    }
}
