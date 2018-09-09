#ifndef LinearBrush_Header
#define LinearBrush_Header


#include "Brush.h"
#include "SimpleBrushPainter.h"

/**
 * @class LinearBrush
 *
 * Linear brush, whose mask has linear falloff.
 */
class LinearBrush
    : public Brush
{

public:
    LinearBrush(BGRA color, int radius);
    virtual ~LinearBrush();

    virtual void brushDown(int x, int y, Canvas2D *canvas);
    virtual void brushDragged(int x, int y, Canvas2D *canvas);
    virtual void brushUp(int x, int y, Canvas2D *canvas);

protected:
    void makeMask(); // Constructs the mask for this brush.

    std::unique_ptr<SimpleBrushPainter> m_painter;
};

#endif
