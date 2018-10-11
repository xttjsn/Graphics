#ifndef FILTERSCALE_H
#define FILTERSCALE_H

#include "filter.h"

class FilterScale : public Filter {
public:

    FilterScale(float scaleX, float scaleY);

    void apply(Canvas2D *canvas) override;

    virtual ~FilterScale() override;

private:

    float m_scaleX;
    float m_scaleY;

    float g(float x, float a);
};

#endif // FILTERSCALE_H
