#include "implicitsphere.h"
#include <algorithm>

ImplicitSphere::ImplicitSphere()
{
}

Intersect ImplicitSphere::intersect(const Ray& ray) {
    float t, x, y, z, sq;
    std::vector<float> ts;
    glm::vec4 start_obj = m_transform_inv * ray.start; // The start point in object space
    glm::vec4 delta_obj = m_transform_inv * ray.delta; // The delta vector in object space
    float px = start_obj.x;
    float py = start_obj.y;
    float pz = start_obj.z;
    float dx = delta_obj.x;
    float dy = delta_obj.y;
    float dz = delta_obj.z;

    float a = dx * dx + dy * dy + dz * dz;
    float b = 2 * (dx * px + dy * py + dz * pz);
    float c = px * px + py * py + pz * pz - 0.25;

    sq = b * b - 4 * a * c;
    if (fequal(sq, 0)) {  // Single solution
        t = -b / (2 * a);
        ts.push_back(t);
    }
    else if (sq > 0) { // Double solution
        t = (-b + glm::sqrt(sq)) / (2 * a);
        ts.push_back(t);

        t = (-b - glm::sqrt(sq)) / (2 * a);
        ts.push_back(t);
    }

    if (ts.empty()) {
        return Intersect(true, glm::vec4(0));
    }

    t = *std::min_element(ts.begin(), ts.end());
    x = px + dx * t; y = py + dy * t; z = pz + dz * t;
    return Intersect(false, m_transform * glm::vec4(x, y, z, 1));
}

glm::vec4 ImplicitSphere::normal(Intersect& intersect) {

}

float ImplicitSphere::surfaceArea() {

}

BoundingBox ImplicitSphere::boundingBox() {

}
