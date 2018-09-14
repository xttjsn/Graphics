#ifndef SIMPEBRUSHPAINTER_H
#define SIMPEBRUSHPAINTER_H

#include "Brush.h"
#include "Canvas2D.h"

class SimpleBrushPainter
{
public:
    SimpleBrushPainter();

protected:
    virtual void paint(std::vector<float>& mask, BGRA bgra, int r, int x, int y, int w, int h, BGRA* pix);

    bool m_useAlphaBlending;
    friend class ConstantBrush;
    friend class LinearBrush;
    friend class QuadraticBrush;
};

#endif // SIMPEBRUSHPAINTER_H
