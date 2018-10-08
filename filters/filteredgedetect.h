#ifndef FILTEREDGEDETECT_H
#define FILTEREDGEDETECT_H

#include "filter.h"

class FilterEdgeDetect : public Filter
{
public:
    FilterEdgeDetect(float sensitivity);

    void apply(Canvas2D * canvas) override;

    virtual ~FilterEdgeDetect() override;
private:
    std::vector<float> m_sobel_kernel_x;
    std::vector<float> m_sobel_kernel_y;
    std::vector<float> m_sobel_kernel_x_h;
    std::vector<float> m_sobel_kernel_x_v;
    std::vector<float> m_sobel_kernel_y_h;
    std::vector<float> m_sobel_kernel_y_v;
    float m_sens;
};

#endif // FILTEREDGEDETECT_H
