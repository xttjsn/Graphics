#ifndef IMPLICITSPHERE_H
#define IMPLICITSPHERE_H


class ImplicitSphere : public ImplicitShape
{
public:
    ImplicitSphere();

    Intersect intersect(const Ray& ray);
    glm::vec4 normal(Intersect& intersect);
    glm::vec4 diffuseAtIntersect(Intersect& intersect, CS123SceneLightData& light);
    float surfaceArea();
    BoundingBox boundingBox();
};

#endif // IMPLICITSPHERE_H
