#include "filterfft.h"
#include "filterscale.h"
#include <iostream>

RGBComplex::RGBComplex()
    : m_r(Complex(0)),
    m_g(Complex(0)),
    m_b(Complex(0)) {}

RGBComplex::RGBComplex(Complex r, Complex g,
                       Complex b)
    : m_r(r),
    m_g(g),
    m_b(b) {}

RGBComplex::RGBComplex(BGRA bgra)
    : m_r(Complex(bgra.r)),
    m_g(Complex(bgra.g)),
    m_b(Complex(bgra.b)) {}

RGBComplex& RGBComplex::operator+=(const RGBComplex& rhs) {
    m_r += rhs.m_r;
    m_g += rhs.m_g;
    m_b += rhs.m_b;
    return *this;
}

void RGBComplex::average() {
    Complex c = R() * 0.299 + G() * 0.587 + B() * 0.114;

    m_r = c;
    m_g = c;
    m_b = c;
}

FilterFFT::FilterFFT(FFTFeature feature)
    : m_feature(feature) {}

FilterFFT::~FilterFFT()
{}

void FilterFFT::separate(Complex *X, int N) {
    std::vector<Complex> tmp;

    tmp.resize(N / 2);

    for (int i = 0; i < N / 2; i++) tmp[i] = X[i * 2 + 1];  // Copy odd to tmp

    for (int i = 0; i < N / 2; i++) X[i] = X[i * 2];        // Move even to front

    for (int i = 0; i < N / 2; i++) X[i + N / 2] = tmp[i];  // Move odd to back
}

void FilterFFT::fft(Complex *X, int N) {
    // Cooley-Tukey FFT
    // X_k = E_k + e^(-2*PI*i*k/N) * O_k
    // X_{k+N/2} = E_k - e^(-2*PI*i*k/N) * O_k
    // This requires that N is a power of 2, which we can guarantee by
    // scaling up the image
    if (N >= 2) {
        // Only perform FFT when N >= 2
        separate(X, N);
        fft(X,         N / 2);
        fft(X + N / 2, N / 2);

        for (int k = 0; k < N / 2; k++) {
            Complex E = X[k];
            Complex O = X[k + N / 2];
            Complex p = std::polar(1.0f, -2.0f * PI * k / N);
            X[k]         = E + O * p;
            X[k + N / 2] = E - O * p;
        }
    }
}

void FilterFFT::ifft(Complex *X, int N) {
    for (int i = 0; i < N; i++) {
        X[i] = std::conj(X[i]);
    }

    fft(X, N);

    for (int i = 0; i < N; i++) {
        X[i] = std::conj(X[i]) / Complex(N);
    }
}

void FilterFFT::fft(RGBComplex *X, int N) {
    std::vector<Complex> Rs, Gs, Bs;

    Rs.resize(N); Gs.resize(N); Bs.resize(N);

    for (int i = 0; i < N; i++) {
        Rs[i] = X[i].R();
        Gs[i] = X[i].G();
        Bs[i] = X[i].B();
    }
    fft(Rs.data(), N);
    fft(Gs.data(), N);
    fft(Bs.data(), N);

    for (int i = 0; i < N; i++) {
        X[i] = RGBComplex(Rs[i], Gs[i], Bs[i]);
    }
}

void FilterFFT::ifft(RGBComplex *X, int N) {
    std::vector<Complex> Rs, Gs, Bs;

    Rs.resize(N); Gs.resize(N); Bs.resize(N);

    for (int i = 0; i < N; i++) {
        Rs[i] = X[i].R();
        Gs[i] = X[i].G();
        Bs[i] = X[i].B();
    }
    ifft(Rs.data(), N);
    ifft(Gs.data(), N);
    ifft(Bs.data(), N);

    for (int i = 0; i < N; i++) {
        X[i] = RGBComplex(Rs[i], Gs[i], Bs[i]);
    }
}

int FilterFFT::nearestPO2(int x) {
    return 1 << static_cast<int>(std::ceil(std::log2(static_cast<double>(x))));
}

void FilterFFT::fft2(std::vector<RGBComplex>& freq, BGRA *data, int nw, int nh) {
    // For every row, do an FFT
    std::vector<RGBComplex> buf;

    buf.resize(nw);

    for (int y = 0; y < nh; y++) {
        // Copy canvas pixel to buf
        for (int x = 0; x < nw; x++) {
            buf[x] = RGBComplex(*(data + y * nw + x));
        }

        // Perform FFT
        fft(buf.data(), nw);

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
        fft(buf.data(), nh);

        // Copy back
        for (int y = 0; y < nh; y++) {
            freq[y * nw + x] = buf[y];
        }
    }
}

void FilterFFT::ifft2(std::vector<RGBComplex>& spatial, std::vector<RGBComplex>& freq, int nw, int nh) {
    std::vector<RGBComplex> buf;

    buf.resize(nw);

    for (int y = 0; y < nh; y++) {
        // Copy "frequency amplitude" to buf
        for (int x = 0; x < nw; x++) {
            buf[x] = freq[y * nw + x];
        }

        // Perform inverse FFT
        ifft(buf.data(), nw);

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
        ifft(buf.data(), nh);

        // Copy back
        for (int y = 0; y < nh; y++) {
            spatial[y * nw + x] = buf[y];
        }
    }
}

void FilterFFT::apply(Canvas2D *canvas) {
    // Unit test
    // test();
    // test1();

    // Convert spatial domain to frequency domain and then convert back
    int w = canvas->width(), h = canvas->height();
    int nw = nearestPO2(w), nh = nearestPO2(h);

    // Create a scale filter to scale the canvas such that both width and height
    // are powers of two.
    FilterScale filterUp(static_cast<double>(nw) / w, static_cast<double>(nh) / h);

    // Create another filter to scale the image down after FFT is done
    FilterScale filterDown(static_cast<double>(w) / nw, static_cast<double>(h) / nh);

    filterUp.apply(canvas);

    BGRA *data = canvas->data(), *cur;

    // Spatial to frequency domain
    std::vector<RGBComplex> freq;
    freq.reserve(nw * nh);
    fft2(freq, data, nw, nh);

    switch (m_feature) {
    case Smoothing:
        smoothing(freq, nw, nh); break;

    case EdgeDetection:
        edgeDetection(freq, nw, nh); break;

    case Sharpening:
        sharpening(freq, nw, nh); break;

    default:
        recover(freq, nw, nh); break;
    }


    // Convert frequency domain to spatial domain
    std::vector<RGBComplex> spatial;
    spatial.reserve(nw * nh);
    ifft2(spatial, freq, nw, nh);

    // Copy spatial to canvas
    for (int y = 0; y < nh; y++) {
        for (int x = 0; x < nw; x++) {
            RGBComplex cplx = spatial[y * nw + x];
            cur = canvas->data() + y * nw + x;

            int R = MIN(MAX(static_cast<int>(std::abs(cplx.R())), 0), 255);
            int G = MIN(MAX(static_cast<int>(std::abs(cplx.G())), 0), 255);
            int B = MIN(MAX(static_cast<int>(std::abs(cplx.B())), 0), 255);

            *(cur) = BGRA(R, G, B, 255);
        }
    }

    // Last step, scale down to original size
    filterDown.apply(canvas);
}

void FilterFFT::recover(std::vector<RGBComplex>& freq, int nw, int nh)       {
    // Do nothing
}

void FilterFFT::smoothing(std::vector<RGBComplex>& freq, int nw, int nh)     {
    // Truncate high frequencies
    double r     = 0.8;
    double limit = r * ((nw - 1) * (nw - 1) + (nh - 1) * (nh - 1));

    for (int u = 0; u < nh; u++) {
        for (int v = 0; v < nw; v++) {
            double dst = u * u + v * v;

            if (dst > limit) freq[u * nw + v] = RGBComplex();
        }
    }
}

void FilterFFT::edgeDetection(std::vector<RGBComplex>& freq, int nw, int nh) {
    // Attenuate low frequencies
    double r     = 0.5;
    double limit = r * ((nw - 1) * (nw - 1) + (nh - 1) * (nh - 1));

    for (int u = 0; u < nh; u++) {
        for (int v = 0; v < nw; v++) {
            double dst = u * u + v * v;

            if (dst < limit) freq[u * nw + v] = RGBComplex();
        }
    }

    // Turn RGB to gray
    for (int u = 0; u < nh; u++) {
        for (int v = 0; v < nw; v++) {
            freq[u * nw + v].average();
        }
    }
}

void FilterFFT::sharpening(std::vector<RGBComplex>& freq, int nw, int nh) {
    // Truncate intermediate frequencies
    double rlo = 0.15;
    double rhi = 0.51;
    double lo  = rlo * ((nw - 1) * (nw - 1) + (nh - 1) * (nh - 1));
    double hi  = rhi * ((nw - 1) * (nw - 1) + (nh - 1) * (nh - 1));

    for (int u = 0; u < nh; u++) {
        for (int v = 0; v < nw; v++) {
            double dst = u * u + v * v;

            if ((dst < hi) && (dst > lo)) freq[u * nw + v] = RGBComplex();
        }
    }
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
    double r = 0.0f, g = 0.0f, b = 0.0f, p = 0.0f;

    for (int u = 0; u < h; u++) {
        for (int v = 0; v < w; v++) {
            // Double nested for loop, slow!
            for (int y = 0; y < h; y++) {
                for (int x = 0; x < w; x++) {
                    cur = getBGRA(data, y, x, w);
                    r   = static_cast<double>(cur->r);
                    g   = static_cast<double>(cur->g);
                    b   = static_cast<double>(cur->b);
                    p   = -2 * PI * (static_cast<double>(v) * x / w + static_cast<double>(u) * y / h);
                    double cos = std::cos(p);
                    double sin = std::sin(p);
                    freq[u * w + v] += RGBComplex(Complex(r * cos, r * sin),
                                                  Complex(g * cos, g * sin),
                                                  Complex(b * cos, b * sin));
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
                    p       = 2 * PI * (static_cast<double>(v) * x / w + static_cast<double>(u) * y / h);
                    double  cos = std::cos(p);
                    double  sin = std::sin(p);
                    Complex f(cos, sin);
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

// void FilterFFT::test() {
//     std::vector<RGBComplex> test = { RGBComplex(1.0, 1.0, 1.0), RGBComplex(1.0, 1.0, 1.0),
//                                      RGBComplex(1.0, 1.0, 1.0), RGBComplex(1.0, 1.0, 1.0),
//                                      RGBComplex(0.0, 0.0, 0.0), RGBComplex(0.0, 0.0, 0.0),
//                                      RGBComplex(0.0, 0.0, 0.0), RGBComplex(0.0, 0.0, 0.0),
//     };
//
//     fft(test.data(), 8);
//
//     printf("fft\n");
//
//     for (int i = 0; i < 8; i++) {
//         printf("R:(%f,%f), G:(%f, %f), B:(%f, %f)\n",
//                test[i].R().real(),
//                test[i].R().imag(),
//                test[i].G().real(),
//                test[i].G().imag(),
//                test[i].B().real(),
//                test[i].B().imag());
//     }
//
//     ifft(test.data(), 8);
//
//     printf("ifft\n");
//
//     for (int i = 0; i < 8; i++) {
//         printf("R:(%f,%f), G:(%f, %f), B:(%f, %f)\n",
//                test[i].R().real(),
//                test[i].R().imag(),
//                test[i].G().real(),
//                test[i].G().imag(),
//                test[i].B().real(),
//                test[i].B().imag());
//     }
// }
//
// void FilterFFT::test1() {
//     std::vector<Complex > test = { 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 0.0, 0.0 };
//
//     fft(test.data(), 8);
//
//     std::cout << "fft" << std::endl;
//
//     for (int i = 0; i < 8; ++i)
//     {
//         std::cout << test[i] << std::endl;
//     }
//
//     // inverse fft
//     ifft(test.data(), 8);
//
//     std::cout << std::endl << "ifft" << std::endl;
//
//     for (int i = 0; i < 8; ++i)
//     {
//         std::cout << test[i] << std::endl;
//     }
// }
