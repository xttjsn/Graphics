#ifndef FILTER_H
#define FILTER_H

#include <vector>
#include "Canvas2D.h"
#include "filterutils.h"

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define PI 3.1415926f

class Filter {
public:

    Filter();
    virtual void apply(Canvas2D *canvas) = 0;
    virtual ~Filter();
};

#endif // FILTER_H
