#ifndef MARQUEECANVAS2D_H
#define MARQUEECANVAS2D_H

#include "BGRA.h"
#include <vector>

class MarqueeCanvas2D
{
public:
    MarqueeCanvas2D();

    void copyFrom(BGRA * data, int startX, int startY, int szX, int szY, int width);
    int width();
    int height();
    BGRA * data();

private:

    std::vector<BGRA> m_data;
    int m_width;
    int m_height;
};

#endif // MARQUEECANVAS2D_H
