#ifndef IMPLICITCYLINDER_H
#define IMPLICITCYLINDER_H


class ImplicitCylinder : public ImplicitShape
{
public:
    ImplicitCylinder();

    Intersect intersect(const Ray& ray);
    glm::vec4 normal(Intersect& intersect);
    float surfaceArea();
    BoundingBox boundingBox();
};

#endif // IMPLICITCYLINDER_H
