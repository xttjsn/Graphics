#ifndef IMPLICITCONE_H
#define IMPLICITCONE_H

#include "implicitshape.h"

class ImplicitCone : public ImplicitShape
{
public:
    ImplicitCone();
    ~ImplicitCone() {}

    Intersect intersect(const Ray& ray) override;
    glm::vec4 normal(const Ray& ray, glm::vec4 pos) override;
    glm::vec2 getUV(glm::vec4 pos, float repeatU, float repeatV) override;
    float surfaceArea() override;
    BoundingBox boundingBox() override;
};

#endif // IMPLICITCONE_H
