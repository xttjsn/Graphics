#ifndef IMPLICITCYLINDER_H
#define IMPLICITCYLINDER_H

#include "implicitshape.h"

class ImplicitCylinder : public ImplicitShape
{
public:
    ImplicitCylinder();
    ~ImplicitCylinder(){}

    Intersect intersect(const Ray& ray) override;
    glm::vec4 normal(const Ray& ray, glm::vec4 pos) override;
    glm::vec2 getUV(glm::vec4 pos, float repeatU, float repeatV) override;
    float surfaceArea() override;
    BoundingBox boundingBox() override;
};

#endif // IMPLICITCYLINDER_H
