#include "implicitcone.h"

ImplicitCone::ImplicitCone()
{
}

Intersect ImplicitCone::intersect(const Ray& ray) {
    float t, x, y, z, t_body, t_cap;
    bool intersectBody = false, intersectCap = false;
    glm::vec4 start_obj = m_transform_inv * ray.start; // The start point in object space
    glm::vec4 delta_obj = m_transform_inv * ray.delta; // The delta vector in object space
    float px = start_obj.x;
    float py = start_obj.y;
    float pz = start_obj.z;
    float dx = delta_obj.x;
    float dy = delta_obj.y;
    float dz = delta_obj.z;

    /******************* Cone Body *********************/
    float a = dy * dy - 4 * (dx * dx + dz * dz);
    float b = -8 *(dx * px+ dz * pz) + dy - 2 * dy * py;
    float c = (py - 0.5) * (py - 0.5) - 4 * (px * px + pz * pz) * (px * pz + pz * pz);

    float t1 = (-b + glm::sqrt(b * b - 4 * a * c)) / (2 * a);
    float t2 = (-b - glm::sqrt(b * b - 4 * a * c)) / (2 * a);

    float y1 = py + t1 * dy;
    float y2 = py + t2 * dy;

    // Both intersections are valid
    if (-0.5 <= y1 && y1 <= 0.5 && -0.5 <= y2 && y2 <= 0.5) {
        t_body = glm::min(t1, t2);
        y = t_body == t1 ? y1 : y2;
        intersectBody = true;
    }
    // Only y1 is valid
    else if (-0.5 <= y1 && y1 <= 0.5) {
        t_body = t1;
        y = y1;
        intersectBody = true;
    }
    // Only y2 is valid
    else if (-0.5 <= y2 && y2 <= 0.5) {
        t_body = t2;
        y = y2;
        intersectBody = true;
    } else {
        // Does not intersect with body
    }

    /******************* Cone Cap ***********************/
    t_cap = -(0.5 + py) / dy;
    float x_intersect = px + dx * t_cap;
    float z_intersect = pz + dz * t_cap;
    float radius = x_intersect * x_intersect + z_intersect * z_intersect;
    if (radius <= 0.25)
        intersectCap = true;

    /****************** Cone Body and Cone Cap *****************/
    if (intersectBody && intersectCap) {
        t = glm::min(t_body, t_cap);
    } else if (intersectBody) {
        t = t_body;
    } else if (intersectCap) {
        t = t_cap;
    } else {
        return Intersect(true, glm::vec4(0));
    }

    x = px + dx * t;
    y = py + dy * t;
    z = pz + dz * t;
    glm::vec4 pos(x, y, z, 1);
    pos = m_transform * pos;
    return Intersect(false, pos);
}
