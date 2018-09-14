#ifndef LINEBRUSHPAINTER_H
#define LINEBRUSHPAINTER_H

#include "Brush.h"
#include "Canvas2D.h"
#include "SimpleBrushPainter.h"

class LineBrushPainter : public SimpleBrushPainter
{
public:
    LineBrushPainter();

protected:
    virtual void paint(BGRA bgra, int x0, int y0, int x1, int y1, Canvas2D* canvas, float offset);

    friend class SpecialBrush1;
};

#endif // LINEBRUSHPAINTER_H
