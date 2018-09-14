#include "SpecialBrush1.h"
#include "glm.hpp"

SpecialBrush1::SpecialBrush1(BGRA color, int radius)
    : Brush(color, radius), m_painter(std::make_unique<LineBrushPainter>())
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
                m_painter->paint(getBGRA(), x, y, col, row, canvas, m_offset);
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
