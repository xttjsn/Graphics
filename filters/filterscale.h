#ifndef FILTERSCALE_H
#define FILTERSCALE_H

#include "filter.h"

class FilterScale : public Filter
{
public:
    FilterScale();

    void apply(Canvas2D * canvas) override;

    virtual ~FilterScale() override;
};

#endif // FILTERSCALE_H
