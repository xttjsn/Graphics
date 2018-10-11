#include "filteredgedetect.h"
#include <cstring>
#include <glm.hpp>
#include "filtergray.h"
#include "marqueecanvas2d.h"

FilterEdgeDetect::FilterEdgeDetect(float sensitivity) : m_sens(sensitivity){
    m_sobel_kernel_x = { -1.0f, 0.0f, 1.0f,
                         -2.0f, 0.0f, 2.0f,
                         -1.0f, 0.0f, 1.0f };
    m_sobel_kernel_y = { -1.0f, -2.0f, -1.0f,
                         0.0f,  0.0f,  0.0f,
                         1.0f,  2.0f,  1.0f };
    m_sobel_kernel_x_h = { -1.0f, 0.0f, 1.0f };
    m_sobel_kernel_x_v = { 1.0f, 2.0f, 1.0f };
    m_sobel_kernel_y_h = { 1.0f, 2.0f, 1.0f };
    m_sobel_kernel_y_v = { -1.0f, 0.0f, 1.0f };
}

FilterEdgeDetect::~FilterEdgeDetect()
{ }

void FilterEdgeDetect::apply(Canvas2D * canvas){
    MarqueeCanvas2D mcanv;

    canvas->getMarqueeCanvas2D(&mcanv);
    int w = canvas->width(), h = canvas->height();

    std::vector<float> gray_img(w * h, 0.0f);

    BGRA * data = canvas->data();

    FilterGray filtergray;

    filtergray.apply(&mcanv);

    FilterUtils::BGRAToFloatVec(gray_img, data, w, h);

    //
    std::vector<float> buf_x(w * h, 0.0f);
    std::vector<float> buf_x_h(w * h, 0.0f);
    std::vector<float> buf_y(w * h, 0.0f);
    std::vector<float> buf_y_h(w * h, 0.0f);

    FilterUtils::Convolve2DGray(buf_x_h, gray_img, w, h, m_sobel_kernel_x_h, FilterUtils::ConvType::I2K1H, false);
    FilterUtils::Convolve2DGray(buf_x, buf_x_h, w, h, m_sobel_kernel_x_v, FilterUtils::ConvType::I2K1V, false);
    FilterUtils::Convolve2DGray(buf_y_h, gray_img, w, h, m_sobel_kernel_y_h, FilterUtils::ConvType::I2K1H, false);
    FilterUtils::Convolve2DGray(buf_y, buf_y_h, w, h, m_sobel_kernel_y_v, FilterUtils::ConvType::I2K1V, false);

    int startX = mcanv.startX(), startY = mcanv.startY(), szX = mcanv.width(), szY = mcanv.height();
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            if (i < startY || i >= (startY + szY) || j < startX || j >= (startX + szX))    // Only replace pixels within selection
                continue;
            int idx         = i * w + j;
            float x2        = buf_x[idx] * buf_x[idx];
            float y2        = buf_y[idx] * buf_y[idx];
            unsigned char c = FilterUtils::REAL2byte(m_sens * glm::sqrt(x2 + y2));
            *(data + idx) = BGRA(c, c, c, 255);
        }
    }
} // FilterEdgeDetect::apply
