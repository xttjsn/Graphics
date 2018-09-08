#ifndef SPECIALBRUSH1_H
#define SPECIALBRUSH1_H

#include "Brush.h"

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
};


#endif // SPECIALBRUSH1_H
