#ifndef ConstantBrush_Header
#define ConstantBrush_Header

#include "Brush.h"

/**
 * @class ConstantBrush
 *
 * Constant brush with equal mask distribution in a circle.
 */
class ConstantBrush
    : public Brush
{

public:
    ConstantBrush(BGRA color, int radius);
    virtual ~ConstantBrush();

    virtual void brushDown(int x, int y, Canvas2D *canvas);
    virtual void brushDragged(int x, int y, Canvas2D *canvas);
    virtual void brushUp(int x, int y, Canvas2D *canvas);

protected:
    void makeMask(); // Constructs the mask for this brush.
    void paintHere(int x, int y, Canvas2D *canvas);
};

#endif
