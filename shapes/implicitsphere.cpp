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
        return Intersect();
    }

    x = px + dx * best_t; y = py + dy * best_t; z = pz + dz * best_t;
    glm::vec4 pos(x, y, z, 1);
    return Intersect(false,
                     m_transform * pos,
                     pos,
                     normal(ray, pos),
                     best_t);
}

glm::vec4 ImplicitSphere::normal(const Ray& ray, glm::vec4 pos) {
    glm::vec4 norm = glm::vec4(glm::normalize(glm::vec3(pos)), 0);
    norm = glm::vec4(glm::normalize(glm::mat3(glm::transpose(m_transform_inv)) * glm::vec3(norm)), 0);

    float delta = glm::dot(ray.delta, norm);
    if (delta > 0)  // delta align with the norm, meaning ray is shooting from inside
        norm = -norm;

    return norm;
}

glm::vec2 ImplicitSphere::getUV(glm::vec4 pos, float repeatU, float repeatV) {

    float u, v;
    glm::vec2 uv;

    float theta = glm::acos(pos.y / 0.5);
    float phi = std::atan2(pos.z, pos.x);

    u = 1.0f - phi / (2.0f * PI);
    v = 1.0f - theta / PI;
    uv = glm::vec2(glm::fract(u * repeatU), glm::fract(v * repeatV));
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
