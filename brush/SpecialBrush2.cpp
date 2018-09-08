#include "SpecialBrush2.h"

SpecialBrush2::SpecialBrush2(BGRA color, int radius)
    : Brush(color, radius)
{
}

SpecialBrush2::~SpecialBrush2()
{
}

void SpecialBrush2::brushDown(int x, int y, Canvas2D *canvas) {}

void SpecialBrush2::brushDragged(int x, int y, Canvas2D *canvas) {}

void SpecialBrush2::brushUp(int x, int y, Canvas2D *canvas) {}

void SpecialBrush2::makeMask() {}
