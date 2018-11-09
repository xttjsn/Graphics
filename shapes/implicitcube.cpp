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
    std::vector<float> ts;

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
    y = py + dy * t; z = pz * dz * t;
    if (-0.5 <= y && y <= 0.5 && -0.5 <= z && z <= 0.5)
        ts.push_back(t);

    // x = 0.5
    t = (0.5 - px) / dx;
    y = py + dy * t; z = pz * dz * t;
    if (-0.5 <= y && y <= 0.5 && -0.5 <= z && z <= 0.5)
        ts.push_back(t);

    // y = -0.5
    t = (-0.5 - py) / dy;
    x = px + dx * t; z = pz * dz * t;
    if (-0.5 <= x && x <= 0.5 && -0.5 <= z && z <= 0.5)
        ts.push_back(t);

    // y = 0.5
    t = (0.5 - py) / dy;
    x = px + dx * t; z = pz * dz * t;
    if (-0.5 <= x && x <= 0.5 && -0.5 <= z && z <= 0.5)
        ts.push_back(t);

    // z = -0.5
    t = (-0.5 - pz) / dz;
    x = px + dx * t; y = py + dy * t;
    if (-0.5 <= x && x <= 0.5 && -0.5 <= y && y <= 0.5)
        ts.push_back(t);

    // z = 0.5
    t = (0.5 - pz) / dz;
    x = px + dx * t; y = py + dy * t;
    if (-0.5 <= x && x <= 0.5 && -0.5 <= y && y <= 0.5)
        ts.push_back(t);

    if (ts.empty()) {
        return Intersect(true, glm::vec4(0), FLT_MAX);
    }

    t = *std::min_element(ts.begin(), ts.end());
    x = px + dx * t; y = py + dy * t; z = pz + dz * t;
    return Intersect(false, m_transform * glm::vec4(x, y, z, 1), t);
}

glm::vec4 ImplicitCube::normal(Intersect& intersect) {
    // Again, assuming that intersect is a valid, don't perform any check
    if (intersect.miss) return glm::vec4(0, 0, 0, 0);

    glm::vec4 pos = intersect.pos, norm;
    pos = m_transform_inv * pos;        // Get intersection point in object space

    //TODO: what about extreme points? Do we average the normal?

    if (fequal(pos.x, -0.5f))
        norm = glm::vec4(-1, 0, 0, 0);
    else if (fequal(pos.x, 0.5f))
        norm = glm::vec4(1, 0, 0, 0);
    else if (fequal(pos.y, -0.5f))
        norm = glm::vec4(0, -1, 0, 0);
    else if (fequal(pos.y, 0.5f))
        norm = glm::vec4(0, 1, 0, 0);
    else if (fequal(pos.z, -0.5f))
        norm = glm::vec4(0, 0, -1, 0);
    else if (fequal(pos.z, 0.5f))
        norm = glm::vec4(0, 0, 1, 0);
    else {
        std::perror("Invalid intersect for Cube.");
        exit(1);
    }

    return glm::normalize(m_transform * norm);
}

float ImplicitCube::surfaceArea() {
    glm::vec4 v0(-0.5, -0.5, -0.5, 1);
    glm::vec4 v1(-0.5, -0.5, 0.5, 1);

    v0 = m_transform_inv * v0;
    v1 = m_transform_inv * v1;

    float dist = glm::distance(v0, v1);

    return dist*dist*6;
}

BoundingBox ImplicitCube::boundingBox() {
    std::vector<glm::vec4> extremes = {
        glm::vec4(-0.5, -0.5, -0.5, 1),
        glm::vec4(-0.5, -0.5, 0.5, 1),
        glm::vec4(-0.5, 0.5, -0.5, 1),
        glm::vec4(-0.5, 0.5, 0.5, 1),
        glm::vec4(0.5, -0.5, -0.5, 1),
        glm::vec4(0.5, -0.5, 0.5, 1),
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
