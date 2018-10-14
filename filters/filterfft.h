#ifndef FILTERFFT_H
#define FILTERFFT_H

#include "filter.h"
#include <complex>

class RGBComplex {
public:

    RGBComplex();
    RGBComplex(std::complex<float> r, std::complex<float> g, std::complex<float> b);

    RGBComplex               & operator+=(const RGBComplex& rhs);

    inline std::complex<float> R() {
        return m_r;
    }

    inline std::complex<float> G() {
        return m_g;
    }

    inline std::complex<float> B() {
        return m_b;
    }

private:

    std::complex<float> m_r;
    std::complex<float> m_g;
    std::complex<float> m_b;
};

class RGBFloat {
public:

    RGBFloat();
    RGBFloat(float r, float g, float b);

private:

    float m_r;
    float m_g;
    float m_b;
};

class FilterFFT : public Filter {
public:

    FilterFFT();
    ~FilterFFT() override;

    void apply(Canvas2D *canvas);
};

#endif // FILTERFFT_H
