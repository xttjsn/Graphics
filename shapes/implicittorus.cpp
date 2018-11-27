#include "implicittorus.h"

ImplicitTorus::ImplicitTorus()
{
}

Intersect ImplicitTorus::intersect(const Ray& ray) {
    return Intersect();
}

glm::vec4 ImplicitTorus::normal(const Ray& ray, glm::vec4 pos, bool& inside) {
    return glm::vec4();
}

glm::vec2 ImplicitTorus::getUV(glm::vec4 pos, float repeatU, float repeatV) {
    return glm::vec2(0);
}

float ImplicitTorus::surfaceArea() {
    return 0;
}

BoundingBox ImplicitTorus::boundingBox() {
    return BoundingBox();
}
