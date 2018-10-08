#ifndef FILTER_H
#define FILTER_H

#include <vector>
#include "Canvas2D.h"
#include "filterutils.h"

class Filter
{
public:
    Filter();
    virtual void apply(Canvas2D *canvas) = 0;
    virtual ~Filter();
};

#endif // FILTER_H
