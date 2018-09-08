#ifndef SPECIALBRUSH2_H
#define SPECIALBRUSH2_H

#include "Brush.h"

/**
 * @class SpecialBrush2
 *
 * TODO: What will the effect be?
 */
class SpecialBrush2 : public Brush
{
public:
    SpecialBrush2(BGRA color, int radius);
    virtual ~SpecialBrush2();

    virtual void brushDown(int x, int y, Canvas2D* canvas);
    virtual void brushDragged(int x, int y, Canvas2D* canvas);
    virtual void brushUp(int x, int y, Canvas2D* canvas);

protected:
    void makeMask();
};


#endif // SPECIALBRUSH2_H
