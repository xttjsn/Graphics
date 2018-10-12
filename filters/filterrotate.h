#ifndef FILTERROTATE_H
#define FILTERROTATE_H

#include "filter.h"

class FilterRotate : public Filter
{
public:
    FilterRotate(int rotateAngle);

    void apply(Canvas2D * canvas) override;

    virtual ~FilterRotate() override;

private:
    float m_angle;

    unsigned char mix(int a, int b, int c, int d, float xfrac, float yfrac);
};

#endif // FILTERROTATE_H
