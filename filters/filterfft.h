#ifndef FILTERFFT_H
#define FILTERFFT_H

#include "filter.h"

typedef struct ComplexFloat_t {
    ComplexFloat_t(float r, float i) : m_real(r), m_imagine(i){ }

    float m_real;
    float m_imagine;
} ComplexFloat;

class RGBComplex {
public:
    RGBComplex(ComplexFloat r, ComplexFloat g, ComplexFloat b)
        : m_r(r), m_g(g), m_b(b){ }

private:
    ComplexFloat m_r;
    ComplexFloat m_g;
    ComplexFloat m_b;
};

class FilterFFT : public Filter
{
public:
    FilterFFT();
    ~FilterFFT() override;

    void apply(Canvas2D * canvas);
};

#endif // FILTERFFT_H
