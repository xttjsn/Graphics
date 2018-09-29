#ifndef CONE_H
#define CONE_H

#include "openglshape.h"

class Cone : public OpenGLShape
{
public:
    Cone(int p1, int p2, float p3);

    virtual void reCalculateVertices();
};

#endif // CONE_H
