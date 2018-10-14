#ifndef FILTERFFT_H
#define FILTERFFT_H

#include "filter.h"
#include <complex>

class RGBComplex {
public:

    RGBComplex();
    RGBComplex(std::complex<float> r, std::complex<float> g, std::complex<float> b);
    RGBComplex(BGRA bgra);

    RGBComplex               & operator+=(const RGBComplex& rhs);
    RGBComplex               & operator+(const RGBComplex& rhs);
    RGBComplex               & operator-(const RGBComplex& rhs);
    RGBComplex               & operator*(const std::complex<float> rhs);

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

class FilterFFT : public Filter {
public:

    FilterFFT();
    ~FilterFFT() override;

    void applyDFT(Canvas2D *canvas);
    void apply(Canvas2D *canvas);

private:

    void separate(RGBComplex *X, int N);
    void fft(RGBComplex *X, int N, bool inverse = false);
    int  nearestPO2(int x);
};

#endif // FILTERFFT_H
