#ifndef FILTERUTILS_H
#define FILTERUTILS_H

#include "BGRA.h"
#include <vector>

namespace FilterUtils {
inline unsigned char REAL2byte(float f);
void Convolve2D(BGRA * dst, BGRA * src, int width, int height, const std::vector<float> &kernel, bool normalize);
void Convolve1DH(BGRA * dst, BGRA * src, int width, int height, const std::vector<float> &kernel, bool normalize);
void Convolve1DV(BGRA * dst, BGRA * src, int width, int height, const std::vector<float> &kernel, bool normalize);
}

#endif // FILTERUTILS_H
