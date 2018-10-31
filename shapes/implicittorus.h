#ifndef IMPLICITTORUS_H
#define IMPLICITTORUS_H


class ImplicitTorus : public ImplicitShape
{
public:
    ImplicitTorus();

    Intersect intersect(const Ray& ray);
    glm::vec4 normal(Intersect& intersect);
    float surfaceArea();
    BoundingBox boundingBox();
};

#endif // IMPLICITTORUS_H
