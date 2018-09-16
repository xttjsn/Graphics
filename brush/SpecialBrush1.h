#ifndef SPECIALBRUSH1_H
#define SPECIALBRUSH1_H

#include "Brush.h"
#include "Canvas2D.h"

/**
 * @class SpecialBrush1
 *
 * TODO: What will the effect be?
 */
class SpecialBrush1 : public Brush
{
public:
    SpecialBrush1(BGRA color, int radius);
    virtual ~SpecialBrush1();

    virtual void brushDown(int x, int y, Canvas2D* canvas);
    virtual void brushDragged(int x, int y, Canvas2D* canvas);
    virtual void brushUp(int x, int y, Canvas2D* canvas);

protected:
    void makeMask();
    void initVisited(Canvas2D* canvas);
    void visitHere(int x, int y, Canvas2D* canvas);
    void paintLine(BGRA bgra, int x0, int y0, int x1, int y1, Canvas2D* canvas, float offset);

    float m_offset = 0.2;
    std::vector<bool> m_visited;
    bool m_visited_inited = false;
};


#endif // SPECIALBRUSH1_H
