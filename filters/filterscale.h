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
};

#endif // FILTERSCALE_H
