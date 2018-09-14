#ifndef QuadraticBrush_Header
#define QuadraticBrush_Header

#include "ConstantBrush.h"

/**
 * @class QuadraticBrush
 *
 * Quadratic brush, whose mask has a falloff of 1/r^2.
 */
class QuadraticBrush
    : public ConstantBrush
{

public:
    QuadraticBrush(BGRA color, int radius);
    virtual ~QuadraticBrush();

protected:
    void makeMask() override; // Constructs the mask for this brush.
};

#endif
