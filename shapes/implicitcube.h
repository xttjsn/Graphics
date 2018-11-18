#ifndef IMPLICITCUBE_H
#define IMPLICITCUBE_H

#include "implicitshape.h"

class ImplicitCube : public ImplicitShape
{
public:
    ImplicitCube();
    ~ImplicitCube() {}

    Intersect intersect(const Ray& ray) override;
    glm::vec4 normal(Intersect& intersect) override;
    glm::vec2 getUV(Intersect& intersect, float repeatU, float repeatV) override;
    float surfaceArea() override;
    BoundingBox boundingBox() override;
};

#endif // IMPLICITCUBE_H
