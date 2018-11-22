#include "implicitsphere.h"
#include <algorithm>

ImplicitSphere::ImplicitSphere()
{
}

Intersect ImplicitSphere::intersect(const Ray& ray) {
    float t, x, y, z, sq;
    float best_t = FLT_MAX;
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
    if (fequal(sq, 0.f)) {  // Single solution
        t = -b / (2 * a);
        if (t > 0)
            best_t = std::min(best_t, t);
    }
    else if (sq > 0) { // Double solution
        t = (-b + glm::sqrt(sq)) / (2 * a);
        if (t > 0)
            best_t = std::min(best_t, t);

        t = (-b - glm::sqrt(sq)) / (2 * a);
        if (t > 0)
            best_t = std::min(best_t, t);
    }

    if (fequal2(best_t, FLT_MAX)) {
        return Intersect(true, glm::vec4(0), FLT_MAX);
    }

    x = px + dx * best_t; y = py + dy * best_t; z = pz + dz * best_t;
    return Intersect(false, m_transform * glm::vec4(x, y, z, 1), best_t);
}

glm::vec4 ImplicitSphere::normal(Intersect& intersect) {
    if (intersect.miss) return glm::vec4(0);

    glm::vec4 pos = intersect.pos, norm;
    pos = m_transform_inv * pos;        // Get intersection point in object space

    norm = glm::vec4(glm::normalize(glm::vec3(pos)), 0);
    return norm;
}

glm::vec2 ImplicitSphere::getUV(Intersect& intersect, float repeatU, float repeatV) {
    if (intersect.miss) return glm::vec2(0);

    glm::vec2 uv;
    glm::vec4 pos = m_transform_inv * intersect.pos;        // Get intersection point in object space

    float theta = glm::acos(pos.y / 0.5);
    float phi = std::atan2(pos.z, pos.x);
    uv = glm::vec2(phi / 2.0f * PI, 1.0f - theta / PI);
    return uv;
}

float ImplicitSphere::surfaceArea() {
    // Sphere might be transformed into an ellipsoid, so we have to figure out the radius
    // in all axes.
    // Then we use the approximation:
    // Area = 4 * PI * ((a^p * b^p + a^p * c^p + b^p * c^p) / 3)^(1/p),
    // Where p = 1.6075
    // Source: https://en.wikipedia.org/wiki/Ellipsoid

    glm::vec4 top(0, 0.5, 0, 1);
    glm::vec4 right(0.5, 0, 0, 1);
    glm::vec4 front(0, 0, 0.5, 1);
    glm::vec4 center(0, 0, 0, 1);

    top = m_transform * top;
    right = m_transform * right;
    front = m_transform * front;
    center = m_transform * center;

    float a = glm::distance(center, right);
    float b = glm::distance(center, top);
    float c = glm::distance(center, front);
    const float p = 1.6075;

    float a_p = glm::pow(a, p);
    float b_p = glm::pow(b, p);
    float c_p = glm::pow(c, p);
    float area = 4 * PI * glm::pow((a_p * b_p + a_p * c_p + b_p * c_p) / 3, 1 / p);
    return area;
}

BoundingBox ImplicitSphere::boundingBox() {
    std::vector<glm::vec4> extremes = {
        // Bottom Extremes
        glm::vec4(-0.5, -0.5, -0.5, 1),    // Left Down
        glm::vec4(-0.5, -0.5, 0.5, 1),     // Left Up
        glm::vec4(0.5, -0.5, -0.5, 1),     // Right Down
        glm::vec4(0.5, -0.5, 0.5, 1),      // Right Up
        // Top Extremes
        glm::vec4(-0.5, 0.5, -0.5, 1),    // Left Down
        glm::vec4(-0.5, 0.5, 0.5, 1),     // Left Up
        glm::vec4(0.5, 0.5, -0.5, 1),     // Right Down
        glm::vec4(0.5, 0.5, 0.5, 1),      // Right Up
    };

    float xMin = FLT_MAX, xMax = -FLT_MAX,
          yMin = FLT_MAX, yMax = -FLT_MAX,
          zMin = FLT_MAX, zMax = -FLT_MAX;
    for (glm::vec4& e : extremes) {
        e = m_transform * e;
        xMin = glm::min(xMin, e.x);
        xMax = glm::max(xMax, e.x);
        yMin = glm::min(yMin, e.y);
        yMax = glm::max(yMax, e.y);
        zMin = glm::min(zMin, e.z);
        zMax = glm::max(zMax, e.z);
    }

    return BoundingBox(xMin, xMax, yMin, yMax, zMin, zMax);
}
