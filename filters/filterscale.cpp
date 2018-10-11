#include "filterscale.h"
#include <cstring>
#include <cmath>

FilterScale::FilterScale(float scaleX, float scaleY)
    : m_scaleX(scaleX), m_scaleY(scaleY)
{}

FilterScale::~FilterScale()
{}

void FilterScale::apply(Canvas2D *canvas) {
    // Get the current image's size and compute the new width and height
    int w = canvas->width(), h = canvas->height();
    int nw = std::ceil(w * m_scaleX), nh = std::ceil(h * m_scaleY);

    // Create two temporary vector, one for horizontally scaled image
    // another for both horizontally and vertically scaled image.
    std::vector<BGRA> himg, hvimg;

    himg.resize(nw, h);
    hvimg.resize(nw, nh);

    // For each pixel in the new canvas, compute accordingly
    // f(x) = x / a + (1 - a) / (2 * a)
    // If a > 0, then support width = 2
    // Else support width = 2 / a
    // Let's use triangle re-sampling

    // Horizontal pass
    for (int r = 0; r < h; r++) {
        for (int c = 0; c < nw; c++) {
            float r_acc = 0, g_acc = 0, b_acc = 0, k = 0, k_acc = 0;
            int   left, right;
            float support = (a > 1) ? 1 : 1 / a;

            float center = c / a + (1 - a) / (2 * a);
            BGRA *cur;

            left  = std::ceil(center - support);
            right = std::floor(center + support);

            for (int cc = left; cc <= right; cc++) {
                if ((cc < 0) || (cc >= w)) continue;
                k      = g(cc - center, a);
                cur    = getBGRA(data, r, cc);
                r_acc += k * cur->r;
                g_acc += k * cur->g;
                b_acc += k * cur->b;
                k_acc += k;
            }
            cur    = getBGRA(data, r, c);
            cur->r = std::round(r_acc / k_acc);
            cur->g = std::round(g_acc / k_acc);
            cur->b = std::round(b_acc / k_acc);
        }
    }

    // Horizontal scale


    // Vertical scale
}

float FilterScale::g(float x, float a) {
    float r = a < 1 ? 1.0 / a : 1.0;

    if ((x < -r) || (x > r)) return 0;

    return (1 - std::fabs(x) / r) / r;
}

float FilterScale::h(int k, float a) {
    float r_acc = 0, g_acc = 0, b_acc = 0, k = 0, k_acc = 0;
    int   left, right;
    float support = (a > 1) ? 1 : 1 / a;
    float center  = k / a + (1 - a) / (2 * a);

    left  = std::ceil(center - support);
    right = std::floor(center + support);

    for (int i = left; i <= right; i++) {
        k = g(i - cente r, a);

        r_acc += k * getPixelR();
        g_acc += k * getPixelG();
        b_acc += k * getPixelB();
        k_acc += k;
    }
    blur
        result = acc / k_acc;
}
