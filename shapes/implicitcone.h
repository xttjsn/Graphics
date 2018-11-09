#ifndef IMPLICITCONE_H
#define IMPLICITCONE_H

#include "implicitshape.h"

class ImplicitCone : public ImplicitShape
{
public:
    ImplicitCone();
    ~ImplicitCone() {}

    Intersect intersect(const Ray& ray) override;
    glm::vec4 normal(Intersect& intersect) override;
    float surfaceArea() override;
    BoundingBox boundingBox() override;
};

#endif // IMPLICITCONE_H
