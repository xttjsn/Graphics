#ifndef FILTERUTILS_H
#define FILTERUTILS_H

#include "BGRA.h"
#include <vector>

namespace FilterUtils {
enum ConvType {
    I2K2,  // 2D image and 2D kernel
    I2K1H, // 2D image and 1D kernel, horizontally
    I2K1V  // 2D image and 1D kernel, vertically
};

inline unsigned char REAL2byte(float f) {
    int i = static_cast<int>((f * 255.0 + 0.5));

    return (i < 0) ? 0 : (i > 255) ? 255 : i;
}
void Convolve2D(BGRA * dst, BGRA * src, int width, int height, const std::vector<float> &kernel, bool normalize);
void Convolve2DGray(std::vector<float> & dst, std::vector<float> & src, int width, int height,
  const std::vector<float> &kernel, ConvType convType, bool normalize);
void Convolve2DGrayHorizontal(std::vector<float> & dst, std::vector<float> & src, int width, int height, const std::vector<float> &kernel, bool normalize);
void Convolve2DGrayVertical(std::vector<float> & dst, std::vector<float> & src, int width, int height, const std::vector<float> &kernel, bool normalize);
void BGRAToFloatVec(std::vector<float> & dst, BGRA * src, int width, int height);
}

#endif // FILTERUTILS_H
