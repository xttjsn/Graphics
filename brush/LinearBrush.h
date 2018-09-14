#ifndef LinearBrush_Header
#define LinearBrush_Header

#include "ConstantBrush.h"

/**
 * @class LinearBrush
 *
 * Linear brush, whose mask has linear falloff.
 */
class LinearBrush
    : public ConstantBrush
{

public:
    LinearBrush(BGRA color, int radius);
    virtual ~LinearBrush();

protected:
    void makeMask() override; // Constructs the mask for this brush.
};

#endif
