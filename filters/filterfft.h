#ifndef FILTERFFT_H
#define FILTERFFT_H

#include "filter.h"
#include <complex>

typedef std::complex<double> Complex;

class RGBComplex {
public:

    RGBComplex();
    RGBComplex(Complex r, Complex g, Complex b);
    RGBComplex(BGRA bgra);

    RGBComplex   & operator+=(const RGBComplex& rhs);

    void           average();

    inline Complex R() {
        return m_r;
    }

    inline Complex G() {
        return m_g;
    }

    inline Complex B() {
        return m_b;
    }

private:

    Complex m_r;
    Complex m_g;
    Complex m_b;
};

enum FFTFeature {
    Smoothing,
    EdgeDetection,
    Sharpening,
    Recover
};

class FilterFFT : public Filter {
public:

    FilterFFT(FFTFeature feature);
    ~FilterFFT() override;

    void applyDFT(Canvas2D *canvas);
    void apply(Canvas2D *canvas);

private:

    // Unit test
    // void test();
    // void test1();
    void separate(Complex *X, int N);
    void fft(Complex *X, int N);
    void ifft(Complex *X, int N);
    void fft(RGBComplex *X, int N);
    void ifft(RGBComplex *X, int N);
    void fft2(std::vector<RGBComplex>& freq, BGRA *data, int nw, int nh);
    void ifft2(std::vector<RGBComplex>& spatial, std::vector<RGBComplex>& freq, int nw, int nh);
    int  nearestPO2(int x);

    void recover(std::vector<RGBComplex>& freq, int nw, int nh);
    void smoothing(std::vector<RGBComplex>& freq, int nw, int nh);
    void edgeDetection(std::vector<RGBComplex>& freq, int nw, int nh);
    void sharpening(std::vector<RGBComplex>& freq, int nw, int nh);

    FFTFeature m_feature;
};

#endif // FILTERFFT_H
