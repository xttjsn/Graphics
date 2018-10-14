#include "filterfft.h"
#include "filterscale.h"

RGBComplex::RGBComplex()
    : m_r(std::complex<float>(0.0f, 0.0f)),
    m_g(std::complex<float>(0.0f, 0.0f)),
    m_b(std::complex<float>(0.0f, 0.0f)) {}

RGBComplex::RGBComplex(std::complex<float> r, std::complex<float> g,
                       std::complex<float> b)
    : m_r(r),
    m_g(g),
    m_b(b) {}

RGBComplex::RGBComplex(BGRA bgra)
    : m_r(std::complex<float>(bgra.r, bgra.r)),
    m_g(std::complex<float>(bgra.g, bgra.g)),
    m_b(std::complex<float>(bgra.b, bgra.b)) {}

RGBComplex& RGBComplex::operator+=(const RGBComplex& rhs) {
    m_r += rhs.m_r;
    m_g += rhs.m_g;
    m_b += rhs.m_b;
    return *this;
}

RGBComplex& RGBComplex::operator+(const RGBComplex& rhs) {
    m_r += rhs.m_r;
    m_g += rhs.m_g;
    m_b += rhs.m_b;
    return *this;
}

RGBComplex& RGBComplex::operator-(const RGBComplex& rhs) {
    m_r -= rhs.m_r;
    m_g -= rhs.m_g;
    m_b -= rhs.m_b;
    return *this;
}

RGBComplex& RGBComplex::operator*(const std::complex<float> rhs) {
    m_r *= rhs;
    m_g *= rhs;
    m_b *= rhs;
    return *this;
}

FilterFFT::FilterFFT()
{}

FilterFFT::~FilterFFT()
{}

void FilterFFT::separate(RGBComplex *X, int N) {
    std::vector<RGBComplex> tmp;

    tmp.resize(N / 2);

    for (int i = 0; i < N / 2; i++) tmp[i] = X[i * 2 + 1];  // Copy odd to tmp

    for (int i = 0; i < N / 2; i++) X[i] = X[i * 2];        // Move even to front

    for (int i = 0; i < N / 2; i++) X[i + N / 2] = tmp[i];  // Move odd to back
}

void FilterFFT::fft(RGBComplex *X, int N, bool inverse) {
    // Cooley-Tukey FFT
    // X_k = E_k + e^(-2*PI*i*k/N) * O_k
    // X_{k+N/2} = E_k - e^(-2*PI*i*k/N) * O_k
    // This requires that N is a power of 2, which we can guarantee by
    // scaling up the image
    if (N >= 2) {
        // Only perform FFT when N >= 2
        separate(X, N);
        fft(X,         N / 2, inverse);
        fft(X + N / 2, N / 2, inverse);

        for (int k = 0; k < N / 2; k++) {
            RGBComplex E = X[k];
            RGBComplex O = X[k + N / 2];
            float angle  = -2.0 * PI * k / N;
            angle *= inverse ? -1 : 1;
            std::complex<float> p(std::cos(angle), std::sin(angle));
            X[k]         = E + O * p;
            X[k + N / 2] = E - O * p;
        }
    }
}

int FilterFFT::nearestPO2(int x) {
    return 1 << static_cast<int>(std::ceil(std::log2(static_cast<double>(x))));
}

void FilterFFT::apply(Canvas2D *canvas) {
    // Convert spatial domain to frequency domain and then convert back
    int w = canvas->width(), h = canvas->height();
    int nw = nearestPO2(w), nh = nearestPO2(h);

    // Create a scale filter to scale the canvas such that both width and height
    // are powers of two.
    FilterScale filterUp(static_cast<float>(nw) / w, static_cast<float>(nh) / h);

    // Create another filter to scale the image down after FFT is done
    FilterScale filterDown(static_cast<float>(w) / nw, static_cast<float>(h) / nh);

    filterUp.apply(canvas);

    BGRA *data = canvas->data(), *cur;

    std::vector<RGBComplex> freq;
    freq.reserve(nw * nh);

    // For every row, do an FFT
    std::vector<RGBComplex> buf;
    buf.resize(nw);

    for (int y = 0; y < nh; y++) {
        // Copy canvas pixel to buf
        for (int x = 0; x < nw; x++) {
            buf[x] = RGBComplex(*(data + y * nw + x));
        }

        // Perform FFT
        fft(buf.data(), nw, false);

        freq.insert(freq.end(), buf.begin(), buf.end());
    }

    // For every column, do an FFT
    buf.resize(nh);

    for (int x = 0; x < nw; x++) {
        // Copy every complex in a column to buf
        for (int y = 0; y < nh; y++) {
            buf[y] = freq[y * nw + x];
        }

        // Perform FFT
        fft(buf.data(), nh, false);

        // Copy back
        for (int y = 0; y < nh; y++) {
            freq[y * nw + x] = buf[y];
        }
    }

    // Convert frequency domain to spatial domain
    // Inverse FFT
    std::vector<RGBComplex> spatial;
    spatial.reserve(nw * nh);

    buf.resize(nw);

    for (int y = 0; y < nh; y++) {
        // Copy "frequency amplitude" to buf
        for (int x = 0; x < nw; x++) {
            buf[x] = freq[y * nw + x];
        }

        // Perform inverse FFT
        fft(buf.data(), nw, true);

        spatial.insert(spatial.end(), buf.begin(), buf.end());
    }

    // For every column, do an inverse FFT
    buf.resize(nh);

    for (int x = 0; x < nw; x++) {
        // Copy every complex in a column to buf
        for (int y = 0; y < nh; y++) {
            buf[y] = spatial[y * nw + x];
        }

        // Perform FFT
        fft(buf.data(), nh, true);

        // Copy back
        for (int y = 0; y < nh; y++) {
            spatial[y * nw + x] = buf[y];
        }
    }

    // Copy spatial to canvas
    for (int y = 0; y < nh; y++) {
        for (int x = 0; x < nw; x++) {
            RGBComplex cplx = spatial[y * nw + x];
            cur = canvas->data() + y * nw + x;

            int R = MIN(MAX(static_cast<int>(std::abs(cplx.R()) / (nw * nh)), 0), 255);
            int G = MIN(MAX(static_cast<int>(std::abs(cplx.G()) / (nw * nh)), 0), 255);
            int B = MIN(MAX(static_cast<int>(std::abs(cplx.B()) / (nw * nh)), 0), 255);

            if ((y % 10 == 0) && (x % 10 == 0)) {
                printf("R:%d, G:%d, B:%d\n", R, G, B);
            }
            *(cur) = BGRA(R, G, B, 255);
        }
    }

    // Last step, scale down to original size
    filterDown.apply(canvas);
}

void FilterFFT::applyDFT(Canvas2D *canvas) {
    // Naive DFT, this is very slow and only serve for my prototyping, do not run it!
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
