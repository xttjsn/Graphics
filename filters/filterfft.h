#ifndef FILTERFFT_H
#define FILTERFFT_H

#include "filter.h"

class ComplexFloat {
public:

    ComplexFloat(float r, float i);

    ComplexFloat& operator+=(const ComplexFloat& rhs);

private:

    float m_real;
    float m_imagine;
};

class RGBComplex {
public:

    RGBComplex();
    RGBComplex(ComplexFloat r, ComplexFloat g, ComplexFloat b);

    RGBComplex& operator+=(const RGBComplex& rhs);

private:

    ComplexFloat m_r;
    ComplexFloat m_g;
    ComplexFloat m_b;
};

class FilterFFT : public Filter {
public:

    FilterFFT();
    ~FilterFFT() override;

    void apply(Canvas2D *canvas);
};

#endif // FILTERFFT_H
