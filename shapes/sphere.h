#ifndef SPHERE_H
#define SPHERE_H

#include "openglshape.h"

class Sphere : public OpenGLShape
{
public:
    Sphere(int p1, int p2, float p3);

    virtual void reCalculateVertices();
};

#endif // SPHERE_H
