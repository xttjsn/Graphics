#include "filterutils.h"
#include <algorithm>
#include "math.h"
#include <cstring>
#include <cstdio>

namespace FilterUtils {
inline unsigned char REAL2byte(float f){
    // int i = static_cast<int>((f * 255.0 + 0.5));
    int i = std::round((f * 255.0f));

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

            // TODO: Task 11 Initialize color to accumulate convolution data
            float red_acc = 0.0f, green_acc = 0.0f, blue_acc = 0.0f;

            // TODO: Task 12
            // Iterate over the kernel using the value from task 10
            // Obtain the value at current index of kernel
            // Find correct index in original image data
            // Accumulate the kernel applied to pixel value in color_acc
            float k = 0.0f, k_acc = 0.0f;
            BGRA * bgra;
            for (int i = r - sz / 2, k_row = 0; i <= r + sz / 2; i++, k_row++) {
                for (int j = c - sz / 2, k_col = 0; j <= c + sz / 2; j++, k_col++) {
                    k = kernel[k_row * sz + k_col];
                    int effective_i = (i < 0 ? 0 : (i >= height ? height - 1 : i));
                    int effective_j = (j < 0 ? 0 : (j >= width ? width - 1 : j));
                    bgra       = src + effective_i * width + effective_j;
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

void Convolve1DH(BGRA * dst, BGRA * src, int width, int height, const std::vector<float> &kernel, bool normalize){
    std::vector<BGRA> buf;
    if (dst == src) {
        buf.reserve(width * height);
    }

    int sz = kernel.size();
    for (int r = 0; r < height; r++) {
        for (int c = 0; c < width; c++) {
            size_t centerIndex = r * width + c;
            float k = 0.0f, k_acc = 0.0f, red_acc = 0.0, green_acc = 0.0f, blue_acc = 0.0f;
            BGRA * bgra;
            for (int i = c - sz / 2, ki = 0; i <= c + sz / 2; i++, ki++) {
                if (i < 0 || i >= width) continue;
                k          = kernel[ki];
                bgra       = src + r * width + i;
                red_acc   += k * static_cast<float>(bgra->r) / 255.f;
                green_acc += k * static_cast<float>(bgra->g) / 255.f;
                blue_acc  += k * static_cast<float>(bgra->b) / 255.f;
                k_acc     += k;
            }

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
} // Convolve1DH

void Convolve1DV(BGRA * dst, BGRA * src, int width, int height, const std::vector<float> &kernel, bool normalize){
    std::vector<BGRA> buf;
    if (dst == src) {
        buf.reserve(width * height);
    }

    int sz = kernel.size();
    for (int r = 0; r < height; r++) {
        for (int c = 0; c < width; c++) {
            size_t centerIndex = r * width + c;
            float k = 0.0f, k_acc = 0.0f, red_acc = 0.0, green_acc = 0.0f, blue_acc = 0.0f;
            BGRA * bgra;
            for (int i = r - sz / 2, ki = 0; i <= r + sz / 2; i++, ki++) {
                if (i < 0 || i >= height) continue;
                k          = kernel[ki];
                bgra       = src + i * width + c;
                red_acc   += k * static_cast<float>(bgra->r) / 255.f;
                green_acc += k * static_cast<float>(bgra->g) / 255.f;
                blue_acc  += k * static_cast<float>(bgra->b) / 255.f;
                k_acc     += k;
            }

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
} // Convolve1DV
}
