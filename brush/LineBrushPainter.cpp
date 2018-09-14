#include "LineBrushPainter.h"
#include "glm.hpp"

LineBrushPainter::LineBrushPainter()
{
}


void LineBrushPainter::paint(BGRA bgra, int x0, int y0, int x1, int y1, Canvas2D* canvas, float offset) {
    // Draw a line between (x0, y0) and (x1, y1)
    BGRA *pix = canvas->data();
    int xstep = x1 - x0 > 0 ? 1 : -1;
    int dx = x1 - x0, dy = y1 - y0;
    int w = canvas->width(), h = canvas->height();
    float srcA;

    int xbegin = glm::round(x0 * (1.f - offset) + x1 * offset);
    int xend = glm::round(x0 * offset + x1 * (1.f - offset));
    for (int x = xbegin; glm::abs(x - xend) > 1; x += xstep) {
        int y = glm::round(y0 + dy * static_cast<float>(x - x0) / static_cast<float>(dx));
        if (y < h && x < w) {
            srcA = bgra.A() / 255.f;
            pix[y * w + x] = pix[y * w + x] * (1.f - srcA) + bgra * srcA;
        }
    }
}
