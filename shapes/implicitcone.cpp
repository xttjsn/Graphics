#include "implicitcone.h"
#include <algorithm>
#include <math.h>

ImplicitCone::ImplicitCone()
{
}

Intersect ImplicitCone::intersect(const Ray& ray) {
    float t, x, y, z, sq;
    float best_t = FLT_MAX;
    glm::vec4 p = m_transform_inv * ray.start; // The start point in object space
    glm::vec4 d = m_transform_inv * ray.delta; // The delta vector in object space

    /******************* Cone Bod.y *********************/
    float a = d.x * d.x + d.z * d.z - 0.25 * d.y * d.y;
    float b = 2 * p.x * d.x + 2 * p.z * d.z - 0.5 * p.y * d.y + 0.25 * d.y;
    float c = p.x * p.x + p.z * p.z - 0.25 * p.y * p.y + 0.25 * p.y - 0.0625;

    sq = b * b - 4 * a * c;
    if (sq > 0) {
        t = (-b + glm::sqrt(sq)) / (2 * a);
        y = p.y + d.y * t;
        if (-0.5 <= y && y <= 0.5 && t > 0)
            best_t = std::min(best_t, t);

        t = (-b - glm::sqrt(sq)) / (2 * a);
        y = p.y + d.y * t;
        if (-0.5 <= y && y <= 0.5 && t > 0)
            best_t = std::min(best_t, t);
    }

    /******************* Cone Cap ***********************/
    t = -(0.5 + p.y) / d.y;
    x = p.x + d.x * t;
    z = p.z + d.z * t;
    float radius_sq = x * x + z * z;
    if (radius_sq <= 0.25 && t > 0)
        best_t = std::min(best_t, t);

    /****************** Take the smallest *****************/

    if (fequal2(best_t, FLT_MAX)) {
        return Intersect();
    }

    x = p.x + d.x * best_t; y = p.y + d.y * best_t; z = p.z + d.z * best_t;
    glm::vec4 pos(x, y, z, 1);
    bool inside;
    glm::vec4 norm = normal(ray, pos, inside);
    return Intersect(false,
                     m_transform * pos,
                     pos,
                     norm,
                     inside,
                     best_t);
}

glm::vec4 ImplicitCone::normal(const Ray& ray, glm::vec4 pos, bool& inside) {

    glm::vec4 norm;

    if (fequal2(pos.y, -0.5f)) {
        // Intersect lies on cone cap
        norm = glm::vec4(0, -1, 0, 0);  // Pointing downwards
    } else {
        // Intersect lies on cone body
        float theta = std::atan2(0.5f, 1.0f);
        float phi = std::atan2(pos.z, pos.x);
        norm = glm::vec4(glm::cos(theta) * glm::cos(phi), glm::sin(theta), glm::cos(theta) * glm::sin(phi), 0);
    }

    norm = glm::vec4(glm::normalize(glm::mat3(glm::transpose(m_transform_inv)) * glm::vec3(norm)), 0);

    float delta = glm::dot(ray.delta, norm);
    if (delta > 0) {  // delta align with the norm, meaning ray is shooting from inside
        norm = -norm;
        inside = true;
    } else
        inside = false;

    return norm;
}

glm::vec2 ImplicitCone::getUV(glm::vec4 pos, float repeatU, float repeatV) {

    float u, v;
    glm::vec2 uv(0);

    if (fequal2(pos.y, -0.5f)) {
        // Intersect lies on cone cap
        u = pos.x + 0.5f;
        v = pos.z + 0.5f;
    } else {
        // Intersect lies on cone body
        float phi = std::atan2(pos.z, pos.x);

        u = 1.0f - phi / (2.0 * PI);
        v = pos.y + 0.5f;
    }

    uv = glm::vec2(glm::fract(u * repeatU), glm::fract(v * repeatV));
    return uv;
}

float ImplicitCone::surfaceArea() {
    // Area = PI * r * (r + sqrt(h^2 + r^2))
    glm::vec4 center(0, -0.5, 0, 1);
    glm::vec4 apex(0, 0.5, 0, 1);
    glm::vec4 edge(0.5, -0.5, 0, 1);

    center = m_transform * center;
    apex = m_transform * apex;
    edge = m_transform * edge;

    float r = glm::distance(center, edge);
    float h = glm::distance(center, apex);
    return PI * r * (r + glm::sqrt(h * h + r * h));
}

BoundingBox ImplicitCone::boundingBox() {
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
