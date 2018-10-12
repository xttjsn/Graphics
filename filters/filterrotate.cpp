#include "filterrotate.h"
#include <cmath>
#include "glm.hpp"
#include "cstring"

FilterRotate::FilterRotate(int rotateAngle) : m_angle(rotateAngle / (2 * PI))
{ }

FilterRotate::~FilterRotate()
{ }

void FilterRotate::apply(Canvas2D * canvas){
    /*
     * The idea is this. For every pixel in the new image, we compute
     * the original location of that pixel in the old image via trigonometric functions.
     * The location we computed probably contains fractions, we then use the fraction
     * to linearly interpolate its actual color with its 4 neighbors.
     * Marquee selection doesn't have effect here.
     */
    int w = canvas->width(), h = canvas->height();
    int nw = std::ceil(std::abs(w * std::cos(m_angle)) + std::abs(h * std::sin(m_angle)));
    int nh = std::ceil(std::abs(w * std::sin(m_angle)) + std::abs(h * std::cos(m_angle)));

    std::vector<BGRA> buffer;
    buffer.resize(nw * nh);

    int centerX = nw / 2, centerY = nh / 2;
    int oldCenterX = w / 2, oldCenterY = h / 2;
    float dst = 0.0f;
    for (int r = 0; r < nh; r++) {
        for (int c = 0; c < nw; c++) {
            // 1. Compute the distance to the center
            int x = c - centerX;
            int y = r - centerY;
            dst = std::sqrt(x * x + y * y);
            // 2. Rotate back
            // x' = x * cos(theta) - y * sin(theta)
            // y' = x * sin(theta) + y * cos(theta)
            float xprime = x * std::cos(m_angle) - y * std::sin(m_angle);
            float yprime = x * std::sin(m_angle) + y * std::cos(m_angle);

            // Set it back to old coordinate
            xprime += oldCenterX;
            yprime += oldCenterY;

            if (xprime < 0 || yprime < 0 || xprime > w - 1 || yprime > h - 1) // Black for pixels outside the original boundary
                continue;

            float xfrac = glm::fract(xprime);
            float yfrac = glm::fract(yprime);

            // Bicubic Interpolation
            xfrac = 3 * xfrac * xfrac - 2 * xfrac * xfrac * xfrac;
            yfrac = 3 * yfrac * yfrac - 2 * yfrac * yfrac * yfrac;

            int xmin = std::floor(xprime),
              ymin   = std::floor(yprime),
              xmax   = std::ceil(xprime),
              ymax   = std::ceil(yprime);

            BGRA * A = getBGRA(canvas->data(), ymin, xmin, canvas->width());
            BGRA * B = getBGRA(canvas->data(), ymin, xmax, canvas->width());
            BGRA * C = getBGRA(canvas->data(), ymax, xmin, canvas->width());
            BGRA * D = getBGRA(canvas->data(), ymax, xmax, canvas->width());

            BGRA * cur = getBGRA(buffer.data(), r, c, nw);

            cur->r = mix(A->r, B->r, C->r, D->r, xfrac, yfrac);
            cur->g = mix(A->g, B->g, C->g, D->g, xfrac, yfrac);
            cur->b = mix(A->b, B->b, C->b, D->b, xfrac, yfrac);
        }
    }

    // Copy buffer to canvas
    canvas->resize(nw, nh);
    std::memcpy(canvas->data(), buffer.data(), sizeof(BGRA) * nw * nh);
} // FilterRotate::apply

unsigned char FilterRotate::mix(int a, int b, int c, int d, float xfrac, float yfrac){
    return MIN(MAX(0,
             glm::round(glm::mix(glm::mix(static_cast<float>(a), static_cast<float>(b), xfrac),
             glm::mix(static_cast<float>(c), static_cast<float>(d),
             xfrac), yfrac))), 255);
}
