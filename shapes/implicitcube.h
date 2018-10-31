#ifndef IMPLICITCUBE_H
#define IMPLICITCUBE_H

#include "implicitshape.h"

class ImplicitCube : public ImplicitShape
{
public:
    ImplicitCube();

    Intersect intersect(const Ray& ray);
    glm::vec4 normal(Intersect& intersect);
    float surfaceArea();
    BoundingBox boundingBox();
};

#endif // IMPLICITCUBE_H
