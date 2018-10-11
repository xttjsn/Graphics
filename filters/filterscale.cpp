#include "filterscale.h"
#include <cstring>
#include <cmath>

FilterScale::FilterScale(float scaleX, float scaleY)
    : m_scaleX(scaleX), m_scaleY(scaleY)
{ }

FilterScale::~FilterScale()
{ }

void FilterScale::apply(Canvas2D * canvas){
    // Get the current image's size and compute the new width and height

    MarqueeCanvas2D mcanv;

    canvas->getMarqueeCanvas2D(&mcanv);
    int w = mcanv.width(), h = mcanv.height();
    int nw = std::ceil(w * m_scaleX), nh = std::ceil(h * m_scaleY);
    BGRA * data = mcanv.data();

    // Create two temporary vector, one for horizontally scaled image
    // another for both horizontally and vertically scaled image.
    std::vector<BGRA> himg, hvimg;

    himg.resize(nw * h);
    hvimg.resize(nw * nh);

    // Horizontal pass
    for (int r = 0; r < h; r++) {
        for (int c = 0; c < nw; c++) {
            float r_acc = 0, g_acc = 0, b_acc = 0, k = 0, k_acc = 0, a = m_scaleX;
            int left, right;
            float support = (a > 1) ? 1 : 1 / a;

            float center = c / a + (1 - a) / (2 * a);
            BGRA * cur;

            left  = std::ceil(center - support);
            right = std::floor(center + support);

            for (int cc = left; cc <= right; cc++) {
                if ((cc < 0) || (cc >= w)) continue;
                k      = g(cc - center, a);
                cur    = getBGRA(data, r, cc, w);
                r_acc += k * cur->r;
                g_acc += k * cur->g;
                b_acc += k * cur->b;
                k_acc += k;
            }
            cur    = getBGRA(himg.data(), r, c, nw);
            cur->r = std::round(r_acc / k_acc);
            cur->g = std::round(g_acc / k_acc);
            cur->b = std::round(b_acc / k_acc);
        }
    }

    // Vertical pass
    for (int r = 0; r < nh; r++) {
        for (int c = 0; c < nw; c++) {
            float r_acc = 0, g_acc = 0, b_acc = 0, k = 0, k_acc = 0, a = m_scaleY;
            int left, right;
            float support = (a > 1) ? 1 : 1 / a;

            float center = r / a + (1 - a) / (2 * a);
            BGRA * cur;

            left  = std::ceil(center - support);
            right = std::floor(center + support);

            for (int rr = left; rr <= right; rr++) {
                if ((rr < 0) || (rr >= h)) continue;
                k      = g(rr - center, a);
                cur    = getBGRA(himg.data(), rr, c, nw);
                r_acc += k * cur->r;
                g_acc += k * cur->g;
                b_acc += k * cur->b;
                k_acc += k;
            }
            cur    = getBGRA(hvimg.data(), r, c, nw);
            cur->r = std::round(r_acc / k_acc);
            cur->g = std::round(g_acc / k_acc);
            cur->b = std::round(b_acc / k_acc);
        }
    }

    // Copy hvimg to canvas
    canvas->resize(nw, nh);
    std::memcpy(canvas->data(), hvimg.data(), sizeof(BGRA) * nw * nh);
} // FilterScale::apply

float FilterScale::g(float x, float a){
    float r = a < 1 ? 1.0 / a : 1.0;

    if ((x < -r) || (x > r)) return 0;

    return (1 - std::fabs(x) / r) / r;
}
