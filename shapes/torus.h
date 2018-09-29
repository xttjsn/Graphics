#ifndef TORUS_H
#define TORUS_H

#include "openglshape.h"

class Torus : public OpenGLShape
{
public:
    Torus(int p1, int p2, float p3);
    void reCalculateVertices();
};

#endif // TORUS_H
