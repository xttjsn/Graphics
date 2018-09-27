#ifndef CYLINDER_H
#define CYLINDER_H

#include "openglshape.h"

class Cylinder : public OpenGLShape
{
public:
    Cylinder(int p1, int p2, int p3);

    virtual void reCalculateVertices();
};

#endif // CYLINDER_H
