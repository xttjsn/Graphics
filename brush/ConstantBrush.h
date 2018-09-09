#ifndef ConstantBrush_Header
#define ConstantBrush_Header

#include "Brush.h"
#include "SimpleBrushPainter.h"

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
    void makeMask(); // Constructs the mask for this brush

    std::unique_ptr<SimpleBrushPainter> m_painter;
};

#endif
