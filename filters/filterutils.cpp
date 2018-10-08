#include "filterutils.h"
#include "filtergray.h"
#include <algorithm>
#include "math.h"
#include <cstring>
#include <cstdio>

namespace FilterUtils {
inline unsigned char REAL2byte(float f){
    int i = static_cast<int>((f * 255.0 + 0.5));

    return (i < 0) ? 0 : (i > 255) ? 255 : i;
}

void Convolve2D(BGRA * dst, BGRA * src, int width, int height, const std::vector<float> &kernel, bool normalize){
    // TODO: Task 9 Create buffer to store new image data
    std::vector<BGRA> buf;
    if (dst == src) {
        buf.reserve(width * height);
    }

    // TODO: Task 10 Obtain kernel dimension
    int sz = std::sqrt(kernel.size());

    for (int r = 0; r < height; r++) {
        for (int c = 0; c < width; c++) {
            size_t centerIndex = r * width + c;

            float k = 0.0f, k_acc = 0.0f, red_acc = 0.0f, green_acc = 0.0f, blue_acc = 0.0f;
            ;
            BGRA * bgra;
            for (int i = r - sz / 2, k_row = 0; i <= r + sz / 2; i++, k_row++) {
                for (int j = c - sz / 2, k_col = 0; j <= c + sz / 2; j++, k_col++) {
                    if (i < 0 || j < 0 || i >= height || j >= width) continue;
                    k          = kernel[k_row * sz + k_col];
                    bgra       = src + i * width + j;
                    red_acc   += k * (static_cast<float>(bgra->r) / 255.f);
                    green_acc += k * (static_cast<float>(bgra->g) / 255.f);
                    blue_acc  += k * (static_cast<float>(bgra->b) / 255.f);
                    k_acc     += k;
                }
            }

            // TODO: Task 13 Ignore outside bounds

            // TODO: Task 14 Update buffer with accumulated color
            float factor = normalize ? k_acc : 1.0f;
            if (dst != src) {
                dst[centerIndex] = BGRA(REAL2byte(red_acc / factor),
                    REAL2byte(green_acc / factor),
                    REAL2byte(blue_acc / factor), 255) + dst[centerIndex];
            } else {
                buf.push_back(BGRA(REAL2byte(red_acc / factor),
                  REAL2byte(green_acc / factor),
                  REAL2byte(blue_acc / factor), 255));
            }
        }
    }

    if (dst == src) {
        std::memcpy(dst, buf.data(), sizeof(BGRA) * width * height);
    }
} // Convolve2D

void Convolve2DGray(std::vector<float> & dst, std::vector<float> & src, int width, int height,
  const std::vector<float> &kernel, ConvType convType, bool normalize){
    int sz = convType == I2K2 ? std::sqrt(kernel.size()) : kernel.size();

    for (int r = 0; r < height; r++) {
        for (int c = 0; c < width; c++) {
            int centerIndex = r * width + c;
            float k = 0.0f, k_acc = 0.0f, gray_acc = 0.0f, gray = 0.0f;

            if (convType == I2K2) {
                for (int i = r - sz / 2, k_row = 0; i <= r + sz / 2; i++, k_row++) {
                    for (int j = c - sz / 2, k_col = 0; j <= c + sz / 2; j++, k_col++) {
                        if (i < 0 || j < 0 || i >= height || j >= width) continue;
                        k         = kernel[k_row * sz + k_col];
                        gray      = src[i * width + j];
                        gray_acc += k * gray;
                        k_acc    += k;
                    }
                }
            } else if (convType == I2K1H) {
                for (int i = c - sz / 2, ki = 0; i <= c + sz / 2; i++, ki++) {
                    if (i < 0 || i >= width) continue;
                    k         = kernel[ki];
                    gray      = src[r * width + i];
                    gray_acc += k * gray;
                    k_acc    += k;
                }
            } else if (convType == I2K1V) {
                for (int i = r - sz / 2, ki = 0; i <= r + sz / 2; i++, ki++) {
                    if (i < 0 || i >= height) continue;
                    k         = kernel[ki];
                    gray      = src[i * width + c];
                    gray_acc += k * gray;
                    k_acc    += k;
                }
            } else {
                perror("Invalid convolution type!");
                exit(-1);
                break;
            }
            float factor = normalize ? k_acc : 1.0f;
            dst[centerIndex] = gray_acc / factor;
        }
    }
} // Convolve2DGray

void BGRAToFloatVec(std::vector<float> & dst, BGRA * src, int width, int height){
    for (int r = 0; r < height; r++) {
        for (int c = 0; c < width; c++) {
            int idx = r * width + c;
            int val = (src + idx)->r;
            dst[idx] = (val / 255.0f);
        }
    }
}
}
