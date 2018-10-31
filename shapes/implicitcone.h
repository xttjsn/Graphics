#ifndef IMPLICITCONE_H
#define IMPLICITCONE_H


class ImplicitCone : public ImplicitShape
{
public:
    ImplicitCone();

    Intersect intersect(const Ray& ray);
    glm::vec4 normal(Intersect& intersect);
    glm::vec4 diffuseAtIntersect(Intersect& intersect, CS123SceneLightData& light);
    float surfaceArea();
    BoundingBox boundingBox();
};

#endif // IMPLICITCONE_H
