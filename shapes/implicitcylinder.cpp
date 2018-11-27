#include "implicitcylinder.h"
#include <algorithm>

ImplicitCylinder::ImplicitCylinder()
{
}

Intersect ImplicitCylinder::intersect(const Ray& ray) {
    // x = px + dx * t; z = pz + dz * t;
    // x^2 + z^2 = 0.25
    // (px^2 + pz^2 - 1/4) + (2*px*dx + 2*pz*dz)t + (dx^2 + dz^2)*t = 0
    // y = py + dy * t;
    // Check if y lies in between -0.5 and 0.5

    float t, x, y, z, radius_sq;
    float best_t = FLT_MAX;
    glm::vec4 start_obj = m_transform_inv * ray.start; // The start point in object space
    glm::vec4 delta_obj = m_transform_inv * ray.delta; // The delta vector in object space
    float px = start_obj.x;
    float py = start_obj.y;
    float pz = start_obj.z;
    float dx = delta_obj.x;
    float dy = delta_obj.y;
    float dz = delta_obj.z;

    /**************** Cylinder Body ****************/
    float a = dx * dx + dz * dz;
    float b = 2 * px * dx + 2 * pz * dz;
    float c = px * px + pz * pz - 0.25;

    float t1 = (-b + glm::sqrt(b * b - 4 * a * c)) / (2 * a);
    float t2 = (-b - glm::sqrt(b * b - 4 * a * c)) / (2 * a);

    y = py + dy * t1;
    if (-0.5 <= y && y <= 0.5 && t1 > 0)
        best_t = std::min(best_t, t1);

    y = py + dy * t2;
    if (-0.5 <= y && y <= 0.5 && t2 > 0)
        best_t = std::min(best_t, t2);

    /************** Cylinder Cap ******************/
    t = -(0.5 + py) / dy;
    x = px + dx * t;
    z = pz + dz * t;
    radius_sq = x * x + z * z;
    if (radius_sq <= 0.25 && t > 0)
        best_t = std::min(best_t, t);

    t = (0.5 - py) / dy;
    x = px + dx * t;
    z = pz + dz * t;
    radius_sq = x * x + z * z;
    if (radius_sq <= 0.25 && t > 0)
        best_t = std::min(best_t, t);

    if (fequal2(best_t, FLT_MAX)) {
        return Intersect(true, glm::vec4(0), FLT_MAX);
    }

    x = px + dx * best_t; y = py + dy * best_t; z = pz + dz * best_t;
    return Intersect(false, m_transform * glm::vec4(x, y, z, 1), best_t);
}

glm::vec4 ImplicitCylinder::normal(const Intersect& intersect) {
    if (intersect.miss) return glm::vec4(0);

    glm::vec4 pos = intersect.pos, norm;
    pos = m_transform_inv * pos;        // Get intersection point in object space

    if (fequal2(pos.y, -0.5f))            // Bottom cap
        norm = glm::normalize(glm::vec4(0, -1, 0, 0));
    else if (fequal2(pos.y, 0.5f))        // Top cap
        norm = glm::normalize(glm::vec4(0, 1, 0, 0));
    else {
        // Intersect lies on cylinder body
        float x = pos.x, z = pos.z;
        float phi = std::atan2(z, x);
        norm = glm::normalize(glm::vec4(glm::cos(phi), 0, glm::sin(phi), 0));
    }

    return norm;
}

glm::vec2 ImplicitCylinder::getUV(const Intersect& intersect, float repeatU, float repeatV) {
    if (intersect.miss) return glm::vec2(0);

    float u, v;
    glm::vec2 uv;
    glm::vec4 pos = m_transform_inv * intersect.pos;        // Get intersection point in object space

    if (fequal2(pos.y, -0.5f)) {
        // Bottom cap
        u = pos.x + 0.5f;
        v = pos.z + 0.5f;
    }
    else if (fequal2(pos.y, 0.5f)) {
        // Top cap
        u = pos.x + 0.5f;
        v = 1.0f - (pos.z + 0.5f);
    }
    else {
        // Intersect lies on cylinder body
        float phi = std::atan2(pos.z, pos.x);

        u = 1.0f - phi / (2.0 * PI);
        v = pos.y + 0.5f;
    }

    uv = glm::vec2(glm::fract(u * repeatU), glm::fract(v * repeatV));
    return uv;
}

float ImplicitCylinder::surfaceArea() {
    // Area = 2 * PI * r * h + 2 * PI * r^2
    glm::vec4 top(0, 0.5, 0, 1);
    glm::vec4 center(0, 0, 0, 1);
    glm::vec4 edge(0.5, 0.5, 0, 1);

    top = m_transform * top;
    center = m_transform * center;
    edge = m_transform * edge;

    float h = 2 * glm::distance(top, center);
    float r = glm::distance(top, edge);

    float area = 2 * PI * r * h + 2 * PI * r * r;
    return area;
}

BoundingBox ImplicitCylinder::boundingBox() {
    std::vector<glm::vec4> extremes = {
        // Bottom Cap Extremes
        glm::vec4(-0.5, -0.5, -0.5, 1),    // Left Down
        glm::vec4(-0.5, -0.5, 0.5, 1),     // Left Up
        glm::vec4(0.5, -0.5, -0.5, 1),     // Right Down
        glm::vec4(0.5, -0.5, 0.5, 1),      // Right Up
        // Top Cap Extremes
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
