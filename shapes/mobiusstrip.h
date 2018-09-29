#ifndef MOBIUSSTRIP_H
#define MOBIUSSTRIP_H

#include "openglshape.h"

class MobiusStrip : public OpenGLShape
{
public:
    MobiusStrip(int p1, int p2, float p3);

    virtual void reCalculateVertices();
};

#endif // MOBIUSSTRIP_H
