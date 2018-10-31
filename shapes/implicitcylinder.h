#ifndef IMPLICITCYLINDER_H
#define IMPLICITCYLINDER_H


class ImplicitCylinder : public ImplicitShape
{
public:
    ImplicitCylinder();

    Intersect intersect(const Ray& ray);
    glm::vec4 normal(Intersect& intersect);
    glm::vec4 diffuseAtIntersect(Intersect& intersect, CS123SceneLightData& light, CS123SceneGlobalData& global);
    float surfaceArea();
    BoundingBox boundingBox();
};

#endif // IMPLICITCYLINDER_H
