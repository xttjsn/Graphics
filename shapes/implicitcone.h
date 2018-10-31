#ifndef IMPLICITCONE_H
#define IMPLICITCONE_H

#include "implicitshape.h"

class ImplicitCone : public ImplicitShape
{
public:
    ImplicitCone();

    Intersect intersect(const Ray& ray);
    glm::vec4 normal(Intersect& intersect);
    float surfaceArea();
    BoundingBox boundingBox();
};

#endif // IMPLICITCONE_H
