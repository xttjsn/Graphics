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
};

#endif // FILTERROTATE_H
