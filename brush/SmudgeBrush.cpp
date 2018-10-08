/**
 * @file   Brush.cpp
 *
 * Implementation of a brush that smudges the image on the canvas as the mouse is dragged.
 *
 * You should fill this file in for the Brush assignment.
 */

#include "SmudgeBrush.h"

#include "Canvas2D.h"
#include "glm.hpp"


SmudgeBrush::SmudgeBrush(BGRA color, int radius) :
    QuadraticBrush(color, radius)
{
    // @TODO: [BRUSH] Initialize any memory you are going to use here. Hint - you are going to
    //       need to store temporary image data in memory. Remember to use automatically managed memory!

    makeMask();
    m_buffer = std::make_unique<BGRA[]>((radius * 2 + 1) * (radius * 2 + 1)); // Make buffer a square of pixels, although the pixels in the corner won't be used.
}


SmudgeBrush::~SmudgeBrush()
{
    // @TODO: [BRUSH] Be sure not to leak memory!  If you're using automatic memory management everywhere,
    //       this will be empty.
}

//! create a mask with a distribution of your choice (probably want to use quadratic for best results)
void SmudgeBrush::makeMask() {
    // @TODO: [BRUSH] Set up the mask for your brush here. For this brush you will probably want
    //        to use a quadratic distribution for the best results. Linear or Gaussian would
    //        work too, however. Feel free to paste your code from the Linear or Quadratic brushes
    //        or modify the class inheritance to be able to take advantage of one of those class's
    //        existing implementations. The choice is yours!
    //
    int r = getRadius();
    m_mask.reserve(r+1);
    std::fill(m_mask.begin(), m_mask.end(), 0.f);
    for (int i = 0; i < getRadius()+1; i++) {
        m_mask[i] = 1.f - static_cast<float>(i) / static_cast<float>(r);
        m_mask[i] *= m_mask[i];
    }
}

void SmudgeBrush::brushDown(int x, int y, Canvas2D *canvas) {
    pickUpPaint(x, y, canvas);
}

//! Picks up paint from the canvas before drawing begins.
void SmudgeBrush::pickUpPaint(int x, int y, Canvas2D* canvas) {

    // @TODO: [BRUSH] Perform the "pick up paint" step described in the assignment handout here.
    //        In other words, you should store the "paint" under the brush mask in a temporary
    //        buffer (which you'll also have to figure out where and how to allocate). Then,
    //        in the paintOnce() method, you'll paste down the paint that you picked up here.
    //
    BGRA *pix = canvas->data();

    int w = canvas->width();
    int h = canvas->height();
    int buffer_row, buffer_col, r = getRadius();

    for (int row = glm::max(0, y - r); row < glm::min(h, y + r + 1); row++) {
        for (int col = glm::max(0, x - r); col < glm::min(w, x + r + 1); col++) {
            int dst = glm::round(glm::sqrt((float)((row - y) * (row - y) + (col - x) * (col - x))));
            if (dst <= r) {
                buffer_row = r - (y - row);
                buffer_col = r - (x - col);
                m_buffer[buffer_row * (2 * r + 1) + buffer_col] = pix[row * w + col];
            }
        }
    }

}

void SmudgeBrush::brushDragged(int mouseX, int mouseY, Canvas2D* canvas) {
    // @TODO: [BRUSH] Here, you'll need to paste down the paint you picked up in
    //        the previous method. Be sure to take the mask into account! You can
    //        ignore the alpha parameter, but you can also use it (smartly) if you
    //        would like to.

    // Put down paint
    BGRA *pix = canvas->data();

    int w = canvas->width();
    int h = canvas->height();
    int r = getRadius();
    int buffer_row, buffer_col, x = mouseX, y = mouseY;

    float mask;

    for (int row = glm::max(0, y - r); row < glm::min(h, y + r + 1); row++) {
        for (int col = glm::max(0, x - r); col < glm::min(w, x + r + 1); col++) {
            int dst = glm::round(glm::sqrt((float)((row - y) * (row - y) + (col - x) * (col - x))));
            if (dst <= r) {
                mask = m_mask[dst];
                buffer_row = r - (y - row);
                buffer_col = r - (x - col);
                pix[row * w + col] = pix[row * w + col] * (1.f - mask) + m_buffer[buffer_row * (2 * r + 1) + buffer_col] * mask;
            }
        }
    }

    // now pick up paint again...
    pickUpPaint(mouseX, mouseY, canvas);

}


