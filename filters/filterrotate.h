#ifndef FILTERROTATE_H
#define FILTERROTATE_H

#include "filter.h"

class FilterRotate : public Filter
{
public:
    FilterRotate();

    void apply(Canvas2D * canvas) override;

    virtual ~FilterRotate() override;
};

#endif // FILTERROTATE_H
