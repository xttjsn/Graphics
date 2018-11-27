#ifndef IMPLICITCONE_H
#define IMPLICITCONE_H

#include "implicitshape.h"

class ImplicitCone : public ImplicitShape
{
public:
    ImplicitCone();
    ~ImplicitCone() {}

    Intersect intersect(const Ray& ray) override;
    glm::vec4 normal(const Intersect& intersect) override;
    glm::vec2 getUV(const Intersect& intersect, float repeatU, float repeatV);
    float surfaceArea() override;
    BoundingBox boundingBox() override;
};

#endif // IMPLICITCONE_H
