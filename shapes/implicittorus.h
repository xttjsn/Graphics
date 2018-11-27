#ifndef IMPLICITTORUS_H
#define IMPLICITTORUS_H

#include "implicitshape.h"

class ImplicitTorus : public ImplicitShape
{
public:
    ImplicitTorus();
    ~ImplicitTorus() {}

    Intersect intersect(const Ray& ray) override;
    glm::vec4 normal(const Intersect& intersect) override;
    glm::vec2 getUV(const Intersect& intersect, float repeatU, float repeatV);
    float surfaceArea() override;
    BoundingBox boundingBox() override;
};

#endif // IMPLICITTORUS_H
