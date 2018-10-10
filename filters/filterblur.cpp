#include "filterblur.h"
#include <cmath>

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

    case BoxGaussian:
        apply_bg(canvas);
        break;

    case FastBox:
        apply_fb(canvas);
        break;

    case Triangle:
        apply_tr(canvas);
        break;

    case TriangleSeparable:
        apply_ts(canvas);
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
            float r_acc = 0.0f, g_acc = 0.0f, b_acc = 0.0f, dst = 0.0f, k = 0.0f,
                  k_acc     = 0.0f;
            int centerIndex = r * w + c;

            for (int rr = r - m_rad; rr < r + m_rad + 1; rr++) {
                for (int cc = c - m_rad; cc < c + m_rad + 1; cc++) {
                    int er = MIN(h - 1, MAX(0, rr));
                    int ec = MIN(w - 1, MAX(0, cc));
                    cur    = data + r * w + ec;
                    r_acc += cur->r;
                    g_acc += cur->g;
                    b_acc += cur->b;
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

void FilterBlur::apply_bg(Canvas2D *canvas) {}

void FilterBlur::apply_fb(Canvas2D *canvas) {}

void FilterBlur::apply_tr(Canvas2D *canvas) {}

void FilterBlur::apply_ts(Canvas2D *canvas) {}

void FilterBlur::calcGaussianKernel() {
    m_gauss_kernel.resize(m_rad * m_rad * 2);
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