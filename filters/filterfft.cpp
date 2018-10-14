#include "filterfft.h"

RGBComplex::RGBComplex()
    : m_r(std::complex<float>(0.0f, 0.0f)),
    m_g(std::complex<float>(0.0f, 0.0f)),
    m_b(std::complex<float>(0.0f, 0.0f)) {}

RGBComplex::RGBComplex(std::complex<float> r, std::complex<float> g,
                       std::complex<float> b)
    : m_r(r),
    m_g(g),
    m_b(b) {}

RGBComplex& RGBComplex::operator+=(const RGBComplex& rhs) {
    m_r += rhs.m_r;
    m_g += rhs.m_g;
    m_b += rhs.m_b;
    return *this;
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
                    p   = -2 * PI * (static_cast<float>(v) * x / w + static_cast<float>(u) * y / h);
                    float cos = std::cos(p);
                    float sin = std::sin(p);
                    freq[u * w + v] += RGBComplex(std::complex<float>(r * cos, r * sin),
                                                  std::complex<float>(g * cos, g * sin),
                                                  std::complex<float>(b * cos, b * sin));
                }
            }
        }
    }

    // Frequency Domain to Spatial Domain
    std::vector<RGBComplex> buf;
    buf.resize(w * h);

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            // Double nested for loop again
            for (int u = 0; u < h; u++) {
                for (int v = 0; v < w; v++) {
                    freqAmp = freq[u * w + v];
                    p       = 2 * PI * (static_cast<float>(v) * x / w + static_cast<float>(u) * y / h);
                    float cos = std::cos(p);
                    float sin = std::sin(p);
                    std::complex<float> f(cos, sin);
                    buf[y * w + x] += RGBComplex(f * freqAmp.R(),
                                                 f * freqAmp.G(),
                                                 f * freqAmp.B());
                }
            }

            // Convert back to unsigned char
            RGBComplex cplx = buf[y * w + x];
            cur    = canvas->data() + y * w + x;
            *(cur) =
                BGRA(MIN(MAX(static_cast<int>(std::abs(cplx.R()) / (w * h)), 0), 255),
                     MIN(MAX(static_cast<int>(std::abs(cplx.G()) / (w * h)), 0), 255),
                     MIN(MAX(static_cast<int>(std::abs(cplx.B()) / (w * h)), 0), 255),
                     255);
        }
    }
} // FilterFFT::apply
