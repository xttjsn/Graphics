#ifndef CONE_H
#define CONE_H

#include "openglshape.h"

class Cone : public OpenGLShape
{
public:
    Cone(int p1, int p2, int p3);

    virtual void reCalculateVertices();
};

#endif // CONE_H
