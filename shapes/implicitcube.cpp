#include "implicitcube.h"
#include <algorithm>

ImplicitCube::ImplicitCube()
{
}

Intersect ImplicitCube::intersect(const Ray& ray) {
    // x = \pm 0.5, px + dx * t = \pm 0.5, t = (\pm 0.5 - px) / dx
    // y = \pm 0.5, py + dy * t = \pm 0.5, t = (\pm 0.5 - py) / dy
    // z = \pm 0.5, pz + dz * t = \pm 0.5, t = (\pm 0.5 - pz) / dz

    float t, x, y, z;
    float best_t = FLT_MAX;

    glm::vec4 start_obj = m_transform_inv * ray.start; // The start point in object space
    glm::vec4 delta_obj = m_transform_inv * ray.delta; // The delta vector in object space
    float px = start_obj.x;
    float py = start_obj.y;
    float pz = start_obj.z;
    float dx = delta_obj.x;
    float dy = delta_obj.y;
    float dz = delta_obj.z;

    // x = -0.5
    t = (-0.5 - px) / dx;
    y = py + dy * t; z = pz + dz * t;
    if (-0.5 <= y && y <= 0.5 && -0.5 <= z && z <= 0.5 && t > 0)
        best_t = std::min(t, best_t);

    // x = 0.5
    t = (0.5 - px) / dx;
    y = py + dy * t; z = pz + dz * t;
    if (-0.5 <= y && y <= 0.5 && -0.5 <= z && z <= 0.5 && t > 0)
        best_t = std::min(t, best_t);

    // y = -0.5
    t = (-0.5 - py) / dy;
    x = px + dx * t; z = pz + dz * t;
    if (-0.5 <= x && x <= 0.5 && -0.5 <= z && z <= 0.5 && t > 0)
        best_t = std::min(t, best_t);

    // y = 0.5
    t = (0.5 - py) / dy;
    x = px + dx * t; z = pz + dz * t;
    if (-0.5 <= x && x <= 0.5 && -0.5 <= z && z <= 0.5 && t > 0)
        best_t = std::min(t, best_t);

    // z = -0.5
    t = (-0.5 - pz) / dz;
    x = px + dx * t; y = py + dy * t;
    if (-0.5 <= x && x <= 0.5 && -0.5 <= y && y <= 0.5 && t > 0)
        best_t = std::min(t, best_t);

    // z = 0.5
    t = (0.5 - pz) / dz;
    x = px + dx * t; y = py + dy * t;
    if (-0.5 <= x && x <= 0.5 && -0.5 <= y && y <= 0.5 && t > 0)
        best_t = std::min(t, best_t);

    if (fequal2(best_t, FLT_MAX)) {
        return Intersect();
    }

    x = px + dx * best_t; y = py + dy * best_t; z = pz + dz * best_t;
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

glm::vec4 ImplicitCube::normal(const Ray& ray, glm::vec4 pos, bool& inside) {
    glm::vec4 norm;

    if (fequal2(pos.x, -0.5f))
        norm = glm::vec4(-1, 0, 0, 0);
    else if (fequal2(pos.x, 0.5f))
        norm = glm::vec4(1, 0, 0, 0);
    else if (fequal2(pos.y, -0.5f))
        norm = glm::vec4(0, -1, 0, 0);
    else if (fequal2(pos.y, 0.5f))
        norm = glm::vec4(0, 1, 0, 0);
    else if (fequal2(pos.z, -0.5f))
        norm = glm::vec4(0, 0, -1, 0);
    else if (fequal2(pos.z, 0.5f))
        norm = glm::vec4(0, 0, 1, 0);
    else {
        std::perror("Invalid intersect for Cube.");
        exit(1);
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

glm::vec2 ImplicitCube::getUV(glm::vec4 pos, float repeatU, float repeatV) {

    float u, v;
    glm::vec2 uv;

    if (fequal2(pos.x, -0.5f)) {
        u = pos.z + 0.5f;
        v = pos.y + 0.5f;
    }
    else if (fequal2(pos.x, 0.5f)) {
        u = 1.0f - (pos.z + 0.5f);
        v = pos.y + 0.5f;
    }
    else if (fequal2(pos.y, -0.5f)) {
        u = pos.x + 0.5f;
        v = pos.z + 0.5f;
    }
    else if (fequal2(pos.y, 0.5f)) {
        u = pos.x + 0.5f;
        v = 1.0f - (pos.z + 0.5f);
    }
    else if (fequal2(pos.z, -0.5f)) {
        u = 1.0f - (pos.x + 0.5f);
        v = pos.y + 0.5f;
    }
    else if (fequal2(pos.z, 0.5f)) {
        u = pos.x + 0.5f;
        v = pos.y + 0.5f;
    }
    else {
        std::perror("Invalid intersect for Cube.");
        exit(1);
    }

    uv = glm::vec2(glm::fract(u * repeatU), glm::fract(v * repeatV));

    return uv;
}

float ImplicitCube::surfaceArea() {
    glm::vec4 v0(-0.5, -0.5, -0.5, 1);
    glm::vec4 v1(-0.5, -0.5, 0.5, 1);

    v0 = m_transform * v0;
    v1 = m_transform * v1;

    float dist = glm::distance(v0, v1);

    return dist*dist*6;
}

BoundingBox ImplicitCube::boundingBox() {
    std::vector<glm::vec4> extremes = {
        glm::vec4(-0.5, -0.5, -0.5, 1),
        glm::vec4(-0.5, -0.5, 0.5, 1),
        glm::vec4(0.5, -0.5, -0.5, 1),
        glm::vec4(0.5, -0.5, 0.5, 1),
        glm::vec4(-0.5, 0.5, -0.5, 1),
        glm::vec4(-0.5, 0.5, 0.5, 1),
        glm::vec4(0.5, 0.5, -0.5, 1),
        glm::vec4(0.5, 0.5, 0.5, 1),
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
