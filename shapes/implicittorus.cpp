#include "implicittorus.h"

ImplicitTorus::ImplicitTorus()
{
}

Intersect ImplicitTorus::intersect(const Ray& ray) {
    return Intersect();
}

glm::vec4 ImplicitTorus::normal(Intersect& intersect) {
    return glm::vec4();
}

float ImplicitTorus::surfaceArea() {
    return 0;
}

BoundingBox ImplicitTorus::boundingBox() {
    return BoundingBox();
}
