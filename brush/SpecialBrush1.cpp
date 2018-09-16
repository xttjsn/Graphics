#include "SpecialBrush1.h"
#include "glm.hpp"

SpecialBrush1::SpecialBrush1(BGRA color, int radius)
    : Brush(color, radius)
{
}

SpecialBrush1::~SpecialBrush1()
{
}

void SpecialBrush1::brushDown(int x, int y, Canvas2D *canvas) {
    visitHere(x, y, canvas);

}

void SpecialBrush1::brushDragged(int x, int y, Canvas2D *canvas) {
    // Find all nearby visited points, draw a line to it (with offset at the beginning and the end)

    int w = canvas->width(), h = canvas->height(), r = getRadius();
    for (int row = glm::max(0, y - r); row < glm::min(h, y + r + 1); row++) {
        for (int col = glm::max(0, x - r); col < glm::min(w, x + r + 1); col++) {
            if (!m_visited[row * w + col]) continue;
            int dst = glm::round(glm::sqrt((float)((row - y) * (row - y) + (col - x) * (col - x))));
            if (dst <= r) {
                paintLine(getBGRA(), x, y, col, row, canvas, m_offset);
            }
        }
    }
    visitHere(x, y, canvas);
}

void SpecialBrush1::brushUp(int x, int y, Canvas2D *canvas) {}

void SpecialBrush1::makeMask() {}

void SpecialBrush1::initVisited(Canvas2D* canvas) {
    int w = canvas->width(), h = canvas->height();
    m_visited.resize(w * h);
    m_visited_inited = true;
}

void SpecialBrush1::visitHere(int x, int y, Canvas2D* canvas) {
    // mark visited
    if (!m_visited_inited) {
        initVisited(canvas);
    }
    m_visited[y * canvas->width() + x] = true;
}

void SpecialBrush1::paintLine(BGRA bgra, int x0, int y0, int x1, int y1, Canvas2D* canvas, float offset) {
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
