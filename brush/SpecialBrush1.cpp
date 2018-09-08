#include "SpecialBrush1.h"

SpecialBrush1::SpecialBrush1(BGRA color, int radius)
    : Brush(color, radius)
{
}

SpecialBrush1::~SpecialBrush1()
{
}

void SpecialBrush1::brushDown(int x, int y, Canvas2D *canvas) {}

void SpecialBrush1::brushDragged(int x, int y, Canvas2D *canvas) {}

void SpecialBrush1::brushUp(int x, int y, Canvas2D *canvas) {}

void SpecialBrush1::makeMask() {}
