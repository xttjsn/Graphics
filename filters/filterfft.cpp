#include "filterfft.h"

ComplexFloat::ComplexFloat(float r, float i)
    : m_real(r),
    m_imagine(i) {}

ComplexFloat& ComplexFloat::operator+=(const ComplexFloat& rhs) {
    m_real    += rhs.m_real;
    m_imagine += rhs.m_imagine;
}

RGBComplex::RGBComplex()
    : m_r(ComplexFloat(0, 0)),
    m_g(ComplexFloat(0, 0)),
    m_b(ComplexFloat(0, 0)) {}

RGBComplex::RGBComplex(ComplexFloat r, ComplexFloat g, ComplexFloat b)
    : m_r(r),
    m_g(g),
    m_b(b) {}

RGBComplex& RGBComplex::operator+=(const RGBComplex& rhs) {
    m_r += rhs.m_r;
    m_g += rhs.m_g;
    m_b += rhs.m_b;
}

FilterFFT::FilterFFT()
{}

FilterFFT::~FilterFFT()
{}

void FilterFFT::apply(Canvas2D *canvas) {
    // Convert spatial domain to frequency domain and then convert back
    int w = canvas->width(), h = canvas->height();

    std::vector<RGBComplex> freq;

    freq.resize(w * h);
    BGRA *data = canvas->data(), *cur;
    RGBComplex freqAmp;

    // Spatial to Frequency Domain
    float r = 0.0f, g = 0.0f, b = 0.0f, p = 0.0f;

    for (int u = 0; u < h; u++) {
        for (int v = 0; v < w; v++) {
            // Double nested for loop, slow!
            for (int y = 0; y < h; y++) {
                for (int x = 0; x < w; x++) {
                    cur = getBGRA(data, y, x, w);
                    r   = static_cast<float>(cur->r);
                    g   = static_cast<float>(cur->g);
                    b   = static_cast<float>(cur->b);
                    p   = -2 * PI *
                          (static_cast<float>(u) * x / w + static_cast<float>(v) *
                           y /
                           h);
                    float cos = std::cos(p);
                    float sin = std::sin(p);
                    freq[u * w + v] += RGBComplex(ComplexFloat(r * cos, r * sin),
                                                  ComplexFloat(g * cos, g * sin),
                                                  ComplexFloat(b * cos, b * sin));
                }
            }
        }
    }

    // Frequency Domain to Spatial Domain
    std::vector<BGRAFloat> bufFloat;
    bufFloat.resize(w * h);

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            // Double nested for loop again
            for (int u = 0; u < h; u++) {
                for (int v = 0; v < w; v++) {
                    freqAmp = freq[u * w + v];
                    p       = 2 * PI *
                              (static_cast<float>(u) * x / w +
                               static_cast<float>(v) * y /
                               h);
                    float cos = std::cos(p);
                    float sin = std::sin(p);
                    ComplexFloat cmp(cos, sin);
                    bufFloat[y * w + x] += BGRAFloat(cmp * freqAmp.R(),
                                                     cmp * freqAmp.G(),
                                                     cmp * freqAmp.B(),
                                                     255);
                }
            }

            bufFloat[y * w + x] /= w * h;
        }
    }

    // Convert float to unsigned char
    BGRAFloat bgraFloat;
    BGRA     *cur;

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            bgraFloat = bufFloat[y * w + x];
            cur       = canvas->data() + y * w + x;
            *(cur)    = BGRA(MIN(MAX(static_cast<int>(bufFloat->r), 0), 255),
                             MIN(MAX(static_cast<int>(bufFloat->g), 0), 255),
                             MIN(MAX(static_cast<int>(bufFloat->b), 0), 255));
        }
    }

    //
} // FilterFFT::apply
