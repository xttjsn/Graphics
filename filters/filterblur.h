#ifndef FILTERBLUR_H
#define FILTERBLUR_H

#include "filter.h"

enum BlurType {
    NaiveGaussian,
    NaiveGaussianSeparable,
    Box,
    BoxGaussian,
    FastBox,
    Triangle,
    TriangleSeparable
};

class FilterBlur : public Filter {
public:

    FilterBlur(int radius, BlurType type);

    void apply(Canvas2D *canvas) override;

    void apply_ng(Canvas2D *canvas);
    void apply_ngs(Canvas2D *canvas);
    void apply_bo(Canvas2D *canvas);
    void apply_bg(Canvas2D *canvas);
    void apply_fb(Canvas2D *canvas);
    void apply_tr(Canvas2D *canvas);
    void apply_ts(Canvas2D *canvas);

    virtual ~FilterBlur() override;

private:

    int m_rad;
    BlurType m_type;
};

#endif // FILTERBLUR_H
