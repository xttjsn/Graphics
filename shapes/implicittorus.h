#ifndef IMPLICITTORUS_H
#define IMPLICITTORUS_H

#include "implicitshape.h"

class ImplicitTorus : public ImplicitShape
{
public:
    ImplicitTorus();
    ~ImplicitTorus() {}

    Intersect intersect(const Ray& ray) override;
    glm::vec4 normal(const Ray& ray, glm::vec4 pos) override;
    glm::vec2 getUV(glm::vec4 pos, float repeatU, float repeatV);
    float surfaceArea() override;
    BoundingBox boundingBox() override;
};

#endif // IMPLICITTORUS_H
