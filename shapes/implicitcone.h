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
    glm::vec2 getUV(Intersect& intersect, float repeatU, float repeatV);
    float surfaceArea() override;
    BoundingBox boundingBox() override;
};

#endif // IMPLICITCONE_H
