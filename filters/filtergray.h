#ifndef FILTERGRAY_H
#define FILTERGRAY_H

#include "filter.h"

class FilterGray : public Filter {
public:
    FilterGray();

    void apply(Canvas2D * canvas) override;

    virtual ~FilterGray() override;
};


#endif // FILTERGRAY_H
