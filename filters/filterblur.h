#ifndef FILTERBLUR_H
#define FILTERBLUR_H

#include "filter.h"

class FilterBlur : public Filter
{
public:
    FilterBlur();

    void apply(Canvas2D *canvas) override;

    virtual ~FilterBlur() override;
};

#endif // FILTERBLUR_H
