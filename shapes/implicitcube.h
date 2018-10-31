#ifndef IMPLICITCUBE_H
#define IMPLICITCUBE_H


class ImplicitCube : public ImplicitShape
{
public:
    ImplicitCube();

    Intersect intersect(const Ray& ray);
    glm::vec4 normal(Intersect& intersect);
    glm::vec4 diffuseAtIntersect(Intersect& intersect, CS123SceneLightData& light);
    float surfaceArea();
    BoundingBox boundingBox();
};

#endif // IMPLICITCUBE_H
