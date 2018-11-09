#ifndef IMPLICITTORUS_H
#define IMPLICITTORUS_H

#include "implicitshape.h"

class ImplicitTorus : public ImplicitShape
{
public:
    ImplicitTorus();
    ~ImplicitTorus() {}

    Intersect intersect(const Ray& ray) override;
    glm::vec4 normal(Intersect& intersect) override;
    float surfaceArea() override;
    BoundingBox boundingBox() override;
};

#endif // IMPLICITTORUS_H
