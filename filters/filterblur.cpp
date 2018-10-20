#include "filterblur.h"
#include "marqueecanvas2d.h"
#include <cmath>
#include "cstring"

FilterBlur::FilterBlur(int radius, BlurType type) : m_rad(radius), m_type(type)
{
    calcGaussianKernel();
}

FilterBlur::~FilterBlur()
{}

void FilterBlur::apply(Canvas2D *canvas) {
    switch (m_type) {
    case NaiveGaussian:
        apply_ng(canvas);
        break;

    case NaiveGaussianSeparable:
        apply_ngs(canvas);
        break;

    case NaiveGaussianPrecomputed:
        apply_ngp(canvas);
        break;

    case NaiveGaussianSeparablePrecomputed:
        apply_ngsp(canvas);
        break;

    case Box:
        apply_bo(canvas);
        break;

    case BoxSeparable:
        apply_bs(canvas);
        break;

    case FastBox:
        apply_fb(canvas);
        break;

    default:
        perror("Invalid blur type!");
        exit(-1);
        break;
    }
}

void FilterBlur::apply_ng(Canvas2D *canvas) {
    int   w = canvas->width(), h = canvas->height();
    BGRA *data = canvas->data(), *cur;

    for (int r = 0; r < h; r++) {
        for (int c = 0; c < w; c++) {
            float r_acc = 0.0f, g_acc = 0.0f, b_acc = 0.0f, dst = 0.0f, k = 0.0f,
                  k_acc     = 0.0f;
            int centerIndex = r * w + c;

            for (int rr = r - m_rad; rr < r + m_rad + 1; rr++) {
                for (int cc = c - m_rad; cc < c + m_rad + 1; cc++) {
                    int er = MIN(h - 1, MAX(0, rr));
                    int ec = MIN(w - 1, MAX(0, cc));
                    dst = (er - r) * (er - r) + (ec - c) * (ec - c);
                    k   = std::exp(-dst / (2 * m_rad * m_rad)) /
                          (PI * 2 * m_rad * m_rad);
                    cur    = data + er * w + ec;
                    r_acc += k * (cur->r);
                    g_acc += k * (cur->g);
                    b_acc += k * (cur->b);
                    k_acc += k;
                }
            }
            cur    = data + centerIndex;
            cur->r = MAX(0, MIN(std::round((r_acc / k_acc)), 255));
            cur->g = MAX(0, MIN(std::round((g_acc / k_acc)), 255));
            cur->b = MAX(0, MIN(std::round((b_acc / k_acc)), 255));
        }
    }
}

void FilterBlur::apply_ngs(Canvas2D *canvas) {
    int   w = canvas->width(), h = canvas->height();
    BGRA *data = canvas->data(), *cur;

    for (int t = 0; t < 2; t++) { // k == 0 -> horizontal, k == 1 -> vertical
        for (int r = 0; r < h; r++) {
            for (int c = 0; c < w; c++) {
                float r_acc = 0.0f, g_acc = 0.0f, b_acc = 0.0f, dst = 0.0f,
                      k         = 0.0f,
                      k_acc     = 0.0f;
                int centerIndex = r * w + c;

                if (t == 0) {
                    for (int cc = c - m_rad; cc < c + m_rad + 1; cc++) {
                        int ec = MIN(w - 1, MAX(0, cc));
                        dst = (ec - c) * (ec - c);
                        k   = std::exp(-dst / (2 * m_rad * m_rad)) /
                              (PI * 2 * m_rad * m_rad);
                        cur    = data + r * w + ec;
                        r_acc += k * (cur->r);
                        g_acc += k * (cur->g);
                        b_acc += k * (cur->b);
                        k_acc += k;
                    }
                }
                else if (t == 1) {
                    for (int rr = r - m_rad; rr < r + m_rad + 1; rr++) {
                        int er = MIN(h - 1, MAX(0, rr));
                        dst = (er - r) * (er - r);
                        k   = std::exp(-dst / (2 * m_rad * m_rad)) /
                              (PI * 2 * m_rad * m_rad);
                        cur    = data + er * w + c;
                        r_acc += k * (cur->r);
                        g_acc += k * (cur->g);
                        b_acc += k * (cur->b);
                        k_acc += k;
                    }
                }
                cur    = data + centerIndex;
                cur->r = MAX(0, MIN(std::round((r_acc / k_acc)), 255));
                cur->g = MAX(0, MIN(std::round((g_acc / k_acc)), 255));
                cur->b = MAX(0, MIN(std::round((b_acc / k_acc)), 255));
            }
        }
    }
}

void FilterBlur::apply_ngp(Canvas2D *canvas) {
    int   w = canvas->width(), h = canvas->height();
    BGRA *data = canvas->data(), *cur;

    for (int r = 0; r < h; r++) {
        for (int c = 0; c < w; c++) {
            float r_acc = 0.0f, g_acc = 0.0f, b_acc = 0.0f, dst = 0.0f, k = 0.0f,
                  k_acc     = 0.0f;
            int centerIndex = r * w + c;

            for (int rr = r - m_rad; rr < r + m_rad + 1; rr++) {
                for (int cc = c - m_rad; cc < c + m_rad + 1; cc++) {
                    int er = MIN(h - 1, MAX(0, rr));
                    int ec = MIN(w - 1, MAX(0, cc));
                    dst    = (er - r) * (er - r) + (ec - c) * (ec - c);
                    k      = m_gauss_kernel[dst];
                    cur    = data + er * w + ec;
                    r_acc += k * (cur->r);
                    g_acc += k * (cur->g);
                    b_acc += k * (cur->b);
                    k_acc += k;
                }
            }
            cur    = data + centerIndex;
            cur->r = MAX(0, MIN(std::round((r_acc / k_acc)), 255));
            cur->g = MAX(0, MIN(std::round((g_acc / k_acc)), 255));
            cur->b = MAX(0, MIN(std::round((b_acc / k_acc)), 255));
        }
    }
}

void FilterBlur::apply_ngsp(Canvas2D *canvas) {
    int   w = canvas->width(), h = canvas->height();
    BGRA *data = canvas->data(), *cur;

    for (int t = 0; t < 2; t++) { // k == 0 -> horizontal, k == 1 -> vertical
        for (int r = 0; r < h; r++) {
            for (int c = 0; c < w; c++) {
                float r_acc = 0.0f, g_acc = 0.0f, b_acc = 0.0f, dst = 0.0f,
                      k         = 0.0f,
                      k_acc     = 0.0f;
                int centerIndex = r * w + c;

                if (t == 0) {
                    for (int cc = c - m_rad; cc < c + m_rad + 1; cc++) {
                        int ec = MIN(w - 1, MAX(0, cc));
                        dst    = (ec - c) * (ec - c);
                        k      = m_gauss_kernel[dst];
                        cur    = data + r * w + ec;
                        r_acc += k * (cur->r);
                        g_acc += k * (cur->g);
                        b_acc += k * (cur->b);
                        k_acc += k;
                    }
                }
                else if (t == 1) {
                    for (int rr = r - m_rad; rr < r + m_rad + 1; rr++) {
                        int er = MIN(h - 1, MAX(0, rr));
                        dst    = (er - r) * (er - r);
                        k      = m_gauss_kernel[dst];
                        cur    = data + er * w + c;
                        r_acc += k * (cur->r);
                        g_acc += k * (cur->g);
                        b_acc += k * (cur->b);
                        k_acc += k;
                    }
                }
                cur    = data + centerIndex;
                cur->r = MAX(0, MIN(std::round((r_acc / k_acc)), 255));
                cur->g = MAX(0, MIN(std::round((g_acc / k_acc)), 255));
                cur->b = MAX(0, MIN(std::round((b_acc / k_acc)), 255));
            }
        }
    }
}

void FilterBlur::apply_bo(Canvas2D *canvas) {
    int   w = canvas->width(), h = canvas->height();
    BGRA *data = canvas->data(), *cur;

    for (int r = 0; r < h; r++) {
        for (int c = 0; c < w; c++) {
            float r_acc = 0.0f, g_acc = 0.0f, b_acc = 0.0f, k_acc = 0.0f;
            int centerIndex = r * w + c;

            for (int rr = r - m_rad; rr < r + m_rad + 1; rr++) {
                for (int cc = c - m_rad; cc < c + m_rad + 1; cc++) {
                    int er = MIN(h - 1, MAX(0, rr));
                    int ec = MIN(w - 1, MAX(0, cc));
                    cur    = data + er * w + ec;
                    r_acc += cur->r;
                    g_acc += cur->g;
                    b_acc += cur->b;
                    k_acc += 1;
                }
            }
            cur    = data + centerIndex;
            cur->r = MAX(0, MIN(std::round((r_acc / k_acc)), 255));
            cur->g = MAX(0, MIN(std::round((g_acc / k_acc)), 255));
            cur->b = MAX(0, MIN(std::round((b_acc / k_acc)), 255));
        }
    }
}

void FilterBlur::apply_bs(Canvas2D *canvas) {
    int   w = canvas->width(), h = canvas->height();
    BGRA *data = canvas->data(), *cur;

    for (int t = 0; t < 2; t++) { // t == 0 -> horizontal, t == 1 -> vertical
        for (int r = 0; r < h; r++) {
            for (int c = 0; c < w; c++) {
                float r_acc = 0.0f, g_acc = 0.0f, b_acc = 0.0f,
                      k_acc     = 0.0f;
                int centerIndex = r * w + c;

                if (t == 0) {
                    for (int cc = c - m_rad; cc < c + m_rad + 1; cc++) {
                        int ec = MIN(w - 1, MAX(0, cc));
                        cur    = data + r * w + ec;
                        r_acc += cur->r;
                        g_acc += cur->g;
                        b_acc += cur->b;
                        k_acc += 1;
                    }
                }
                else if (t == 1) {
                    for (int rr = r - m_rad; rr < r + m_rad + 1; rr++) {
                        int er = MIN(h - 1, MAX(0, rr));
                        cur    = data + er * w + c;
                        r_acc += cur->r;
                        g_acc += cur->g;
                        b_acc += cur->b;
                        k_acc += 1;
                    }
                }

                cur    = data + centerIndex;
                cur->r = MAX(0, MIN(std::round((r_acc / k_acc)), 255));
                cur->g = MAX(0, MIN(std::round((g_acc / k_acc)), 255));
                cur->b = MAX(0, MIN(std::round((b_acc / k_acc)), 255));
            }
        }
    }
}

void FilterBlur::apply_fb(Canvas2D *canvas) {
    /*
     * The idea is to pre-compute the accumulation of pixel values.
     * For horizontal pass, when we need to perform convolution for
     * a certain position for red channel, we compute
     *     v(row, col) = v(row, col - 1) - r(row, col - radius) + r(row, col)
     * where v(row, col) is the value we already computed for position row, col
     * and r(row, col) is the value for red channel at row, col
     * For vertical pass, we compute
     *     v(row, col) = v(row - 1, col) - r(row - radius, col) + r(row, col)
     */

    int   w = canvas->width(), h = canvas->height();
    MarqueeCanvas2D mcanv;
    canvas->getMarqueeCanvas2D(&mcanv);
    std::vector<BGRA> buffer;
    buffer.resize(w * h);
    std::memcpy(buffer.data(), canvas->data(), sizeof(BGRA) * w * h);
    BGRA *data = buffer.data(), *cur, *ncur;

    // Horizontal pass
    for (int r = 0; r < h; r++) {
        float r_acc = 0.0f, g_acc = 0.0f, b_acc = 0.0f, k_acc = 0.0f;
        int   lr = 0, lg = 0, lb = 0, rr = 0, rg = 0, rb = 0;

        for (int c = 0; c < w; c++) {
            cur = data + r * w + c;

            if (c == 0) {
                // Only does one nested loop if we are at the beginning
                for (int cc = 0; cc < MIN(w, m_rad + 1); cc++) {
                    ncur   = data + r * w + cc;
                    r_acc += ncur->r; g_acc += ncur->g; b_acc += ncur->b;
                    k_acc += 1;
                }
            } else {
                // Figure out the left pixel that we are going to discard
                if (c - m_rad - 1 >= 0) {
                    ncur   = data + r * w + c - m_rad - 1;
                    lr     = ncur->r; lg = ncur->g; lb = ncur->b;
                    k_acc -= 1;
                } else {
                    lr = 0; lg = 0; lb = 0;
                }

                // Figure out the right pixel that we are going to add
                if (c + m_rad < w) {
                    ncur   = data + r * w + c + m_rad;
                    rr     = ncur->r; rg = ncur->g; rb = ncur->b;
                    k_acc += 1;
                } else {
                    rr = 0; rg = 0; rb = 0;
                }

                r_acc = r_acc - lr + rr;
                g_acc = g_acc - lg + rg;
                b_acc = b_acc - lb + rb;
            }

            cur->r = MAX(0, MIN(std::round((r_acc / k_acc)), 255));
            cur->g = MAX(0, MIN(std::round((g_acc / k_acc)), 255));
            cur->b = MAX(0, MIN(std::round((b_acc / k_acc)), 255));
        }
    }

    // Vertical pass, basically symmetric code as above
    // TODO: find a way to combine them
    for (int c = 0; c < w; c++) {
        float r_acc = 0.0f, g_acc = 0.0f, b_acc = 0.0f, k_acc = 0.0f;
        int   lr = 0, lg = 0, lb = 0, rr = 0, rg = 0, rb = 0;

        for (int r = 0; r < h; r++) {
            cur = data + r * w + c;

            if (r == 0) {
                for (int rr = 0; rr < MIN(h, m_rad + 1); rr++) {
                    ncur   = data + rr * w + c;
                    r_acc += ncur->r; g_acc += ncur->g; b_acc += ncur->b;
                    k_acc += 1;
                }
            } else {
                if (r - m_rad - 1 >= 0) {
                    ncur   = data + (r - m_rad - 1) * w + c;
                    lr     = ncur->r; lg = ncur->g; lb = ncur->b;
                    k_acc -= 1;
                } else {
                    lr = 0; lg = 0; lb = 0;
                }

                if (r + m_rad < h) {
                    ncur   = data + (r + m_rad) * w + c;
                    rr     = ncur->r; rg = ncur->g; rb = ncur->b;
                    k_acc += 1;
                } else {
                    rr = 0; rg = 0; rb = 0;
                }

                r_acc = r_acc - lr + rr;
                g_acc = g_acc - lg + rg;
                b_acc = b_acc - lb + rb;
            }

            cur->r = MAX(0, MIN(std::round((r_acc / k_acc)), 255));
            cur->g = MAX(0, MIN(std::round((g_acc / k_acc)), 255));
            cur->b = MAX(0, MIN(std::round((b_acc / k_acc)), 255));
        }
    }

    // Copy pixels to selection
    int startX = mcanv.startX(), startY = mcanv.startY(), szX = mcanv.width(), szY = mcanv.height();
    for (int r = 0; r < h; r++) {
        for (int c = 0; c < w; c++) {
            if (r < startY || r >= (startY + szY) || c < startX || c >= (startX + szX))
                continue;
            cur = canvas->data() + r * w + c;
            (*cur) = *(data + r * w + c);
        }
    }
}

void FilterBlur::calcGaussianKernel() {
    m_gauss_kernel.resize(m_rad * m_rad * 2 + 1);
    int   dst = 0;
    float k   = 0.0f;

    for (int r = -m_rad; r < m_rad + 1; r++) {
        for (int c = -m_rad; c < m_rad + 1; c++) {
            dst = r * r + c * c;
            k   = std::exp(-dst / (2 * m_rad * m_rad)) /
                  (PI * 2 * m_rad * m_rad);
            m_gauss_kernel[dst] = k;
        }
    }
}
