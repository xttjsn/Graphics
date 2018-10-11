#ifndef FILTERGRAY_H
#define FILTERGRAY_H

#include "filter.h"
#include "marqueecanvas2d.h"

class FilterGray : public Filter {
public:
    FilterGray();

    void apply(MarqueeCanvas2D * mcanv);
    void apply(Canvas2D * canvas);

    virtual ~FilterGray() override;
};


#endif // FILTERGRAY_H
