#ifndef FILTERBLUR_H
#define FILTERBLUR_H

#include "filter.h"

enum BlurType {
    NaiveGaussian,
    NaiveGaussianSeparable,
    NaiveGaussianPrecomputed,
    NaiveGaussianSeparablePrecomputed,
    Box,
    BoxSeparable,
    FastBox
};

class FilterBlur : public Filter {
public:

    FilterBlur(int radius, BlurType type);

    void apply(Canvas2D *canvas) override;

    void apply_ng(Canvas2D *canvas);
    void apply_ngs(Canvas2D *canvas);
    void apply_ngp(Canvas2D *canvas);
    void apply_ngsp(Canvas2D *canvas);
    void apply_bo(Canvas2D *canvas);
    void apply_bs(Canvas2D *canvas);
    void apply_fb(Canvas2D *canvas);

    virtual ~FilterBlur() override;

private:

    int m_rad;
    BlurType m_type;
    std::vector<float>m_gauss_kernel;
    std::vector<float>m_box_kernel;

    void calcGaussianKernel();
};

#endif // FILTERBLUR_H
