#ifndef IMPLICITCYLINDER_H
#define IMPLICITCYLINDER_H

#include "implicitshape.h"

class ImplicitCylinder : public ImplicitShape
{
public:
    ImplicitCylinder();
    ~ImplicitCylinder(){}

    Intersect intersect(const Ray& ray) override;
    glm::vec4 normal(Intersect& intersect) override;
    glm::vec2 getUV(Intersect& intersect, float repeatU, float repeatV);
    float surfaceArea() override;
    BoundingBox boundingBox() override;
};

#endif // IMPLICITCYLINDER_H
