#ifndef IMPLICITTORUS_H
#define IMPLICITTORUS_H


class ImplicitTorus : public ImplicitShape
{
public:
    ImplicitTorus();

    Intersect intersect(const Ray& ray);
    glm::vec4 normal(Intersect& intersect);
    glm::vec4 diffuseAtIntersect(Intersect& intersect, CS123SceneLightData& light);
    float surfaceArea();
    BoundingBox boundingBox();
};

#endif // IMPLICITTORUS_H
