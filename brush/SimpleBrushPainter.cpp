#include "SimpleBrushPainter.h"
#include "glm.hpp"

SimpleBrushPainter::SimpleBrushPainter()
{
}


void SimpleBrushPainter::paint(std::vector<float>& masks, BGRA bgra, int r, int x, int y, Canvas2D* canvas) {
    BGRA *pix = canvas->data();

    int w = canvas->width();
    int h = canvas->height();

    float mask, srcA, dstA, outA;

    for (int row = glm::max(0, y - r); row < glm::min(h, y + r + 1); row++) {
        for (int col = glm::max(0, x - r); col < glm::min(w, x + r + 1); col++) {
            int dst = glm::round(glm::sqrt((float)((row - y) * (row - y) + (col - x) * (col - x))));
            if (dst <= r) {
                mask = masks[dst];
                srcA = bgra.A() / 255.f;
                pix[row * w + col] = pix[row * w + col] * (1.f - mask * srcA) + bgra * mask * srcA;
            }
        }
    }

    canvas->update();
}
