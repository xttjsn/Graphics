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

    std::vector<float> ts;
    float t, x, y, z, radius_sq;
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
    if (-0.5 <= y && y <= 0.5)
        ts.push_back(t1);

    y = py + dy * t2;
    if (-0.5 <= y && y <= 0.5)
        ts.push_back(t2);

    /************** Cylinder Cap ******************/
    t = -(0.5 + py) / dy;
    x = px + dx * t;
    z = pz + dz * t;
    radius_sq = x * x + z * z;
    if (radius_sq <= 0.25)
        ts.push_back(t);

    t = (0.5 - py) / dy;
    x = px + dx * t;
    z = pz + dz * t;
    radius_sq = x * x + z * z;
    if (radius_sq <= 0.25)
        ts.push_back(t);

    if (ts.empty()) {
        return Intersect(true, glm::vec4(0));
    }

    t = *std::min_element(ts.begin(), ts.end());
    x = px + dx * t; y = py + dy * t; z = pz + dz * t;
    return Intersect(false, m_transform * glm::vec4(x, y, z, 1));
}

glm::vec4 ImplicitCylinder::normal(Intersect& intersect) {
    if (intersect.miss) return glm::vec4(0);

    glm::vec4 pos = intersect.pos, norm;
    pos = m_transform_inv * pos;        // Get intersection point in object space

    if (fequal(pos.y, -0.5))            // Bottom cap
        norm = glm::vec4(0, -1, 0, 0);
    else if (fequal(pos.y, 0.5))        // Top cap
        norm = glm::vec4(0, 1, 0, 0);
    else {
        // Intersect lies on cylinder body
        float x = pos.x, y = pos.y, z = pos.z;
        float phi = glm::tanh(z / x);
        norm = glm::normalize(glm::vec4(glm::cos(phi + PI / 2.0), 0, glm::sin(phi + PI / 2.0), 0));
    }

    norm = glm::normalize(m_transform * norm);  // Convert back to world space
    return norm;
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
        glm::vec4(-0.5, -0.5, 0, 1),    // Left
        glm::vec4(0, -0.5, -0.5, 1),    // Down
        glm::vec4(0.5, -0.5, 0, 1),     // Right
        glm::vec4(0, -0.5, 0.5, 1),     // Up
        glm::vec4(0.5 * glm::cos(PI / 4), -0.5, 0.5 * glm::sin(PI / 4), 1), // Right Up
        glm::vec4(0.5 * glm::cos(3 * PI / 4), -0.5, 0.5 * glm::sin(3 * PI / 4), 1), // Left Up
        glm::vec4(0.5 * glm::cos(5 * PI / 4), -0.5, 0.5 * glm::sin(5 * PI / 4), 1), // Left Down
        glm::vec4(0.5 * glm::cos(7 * PI / 4), -0.5, 0.5 * glm::sin(7 * PI / 4), 1), // Right Down
        // Top Cap Extremes
        glm::vec4(-0.5, 0.5, 0, 1),    // Left
        glm::vec4(0, 0.5, -0.5, 1),    // Down
        glm::vec4(0.5, 0.5, 0, 1),     // Right
        glm::vec4(0, 0.5, 0.5, 1),     // Up
        glm::vec4(0.5 * glm::cos(PI / 4), 0.5, 0.5 * glm::sin(PI / 4), 1), // Right Up
        glm::vec4(0.5 * glm::cos(3 * PI / 4), 0.5, 0.5 * glm::sin(3 * PI / 4), 1), // Left Up
        glm::vec4(0.5 * glm::cos(5 * PI / 4), 0.5, 0.5 * glm::sin(5 * PI / 4), 1), // Left Down
        glm::vec4(0.5 * glm::cos(7 * PI / 4), 0.5, 0.5 * glm::sin(7 * PI / 4), 1), // Right Down
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
