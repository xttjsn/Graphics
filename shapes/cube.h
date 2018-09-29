#ifndef CUBE_H
#define CUBE_H

#include "openglshape.h"

class Cube : public OpenGLShape
{
public:
    Cube(int p1, int p2, float p3);

    virtual void reCalculateVertices();
};

#endif // CUBE_H
