#ifndef IMPLICITSPHERE_H
#define IMPLICITSPHERE_H

#include "implicitshape.h"

class ImplicitSphere : public ImplicitShape
{
public:
    ImplicitSphere();
    ~ImplicitSphere() {}

    Intersect intersect(const Ray& ray) override;
    glm::vec4 normal(Intersect& intersect) override;
    glm::vec2 getUV(Intersect& intersect, float repeatU, float repeatV);
    float surfaceArea() override;
    BoundingBox boundingBox() override;
};

#endif // IMPLICITSPHERE_H
