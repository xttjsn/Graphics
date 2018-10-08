#include "filteredgedetect.h"
#include <cstring>
#include <glm.hpp>
#include "filtergray.h"

FilterEdgeDetect::FilterEdgeDetect(float sensitivity) : m_sens(sensitivity){
    m_sobel_kernel_x = { -1.0f, 0.0f, 1.0f,
                         -2.0f, 0.0f, 2.0f,
                         -1.0f, 0.0f, 1.0f };
    m_sobel_kernel_y = { -1.0f, -2.0f, -1.0f,
                         0.0f,  0.0f,  0.0f,
                         1.0f,  2.0f,  1.0f };
    m_sobel_kernel_x_h = { -1.0F, 0.0f, 1.0f };
    m_sobel_kernel_x_v = { 1.0F, 2.0f, 1.0f };
    m_sobel_kernel_y_h = { 1.0F, 2.0f, 1.0f };
    m_sobel_kernel_y_v = { -1.0F, 0.0f, 1.0f };
}

FilterEdgeDetect::~FilterEdgeDetect()
{ }

void FilterEdgeDetect::apply(Canvas2D * canvas){
    FilterGray filtergray;

    filtergray.apply(canvas);

    int w = canvas->width(), h = canvas->height();
    BGRA * data = canvas->data();
    //
    std::vector<BGRA> buf_x(w * h, BGRA(0, 0, 0, 0));
    std::vector<BGRA> buf_y(w * h, BGRA(0, 0, 0, 0));
//    FilterUtils::Convolve2D(buf_y.data(), data, w, h, m_sobel_kernel_y, false);
    FilterUtils::Convolve2D(buf_x.data(), data, w, h, m_sobel_kernel_x, false);
    // FilterUtils::Convolve1DH(buf_x.data(), data, w, h, m_sobel_kernel_x_h, false);
    // FilterUtils::Convolve1DV(buf_x.data(), buf_x.data(), w, h, m_sobel_kernel_x_v, false);
    // FilterUtils::Convolve1DH(buf_y.data(), data, w, h, m_sobel_kernel_y_h, false);
    // FilterUtils::Convolve1DV(buf_y.data(), buf_y.data(), w, h, m_sobel_kernel_y_v, false);


    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            int idx         = i * w + j;
            int x2          = buf_x[idx].r * buf_x[idx].r;
            int y2          = buf_y[idx].r * buf_y[idx].r;
            unsigned char c = glm::min(static_cast<int>(glm::round(m_sens * glm::sqrt(
                    static_cast<double>(x2 + y2)))), 255);
            *(data + idx) = BGRA(c, c, c, 255);
        }
    }
} // FilterEdgeDetect::apply
