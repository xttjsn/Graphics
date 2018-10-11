#include "marqueecanvas2d.h"

MarqueeCanvas2D::MarqueeCanvas2D()
{ }

void MarqueeCanvas2D::copyFrom(BGRA * data, int startX, int startY, int szX, int szY, int width){
    m_data.reserve(szX * szY);
    m_width = szX, m_height = szY;
    for (int r = startY; r < startY + szY; r++) {
        int rowStart = r * width;
        for (int c = startX; c < startX + szX; c++) {
            m_data.push_back(*(data + rowStart + c));
        }
    }
}

int MarqueeCanvas2D::width(){
    return m_width;
}

int MarqueeCanvas2D::height(){
    return m_height;
}

BGRA * MarqueeCanvas2D::data(){
    return m_data.data();
}
