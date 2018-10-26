#include "sphere.h"
#include "glm.hpp"
#include "glm/gtx/transform.hpp"
#include "shapeutil.h"

Sphere::Sphere(int p1, int p2, float p3) :
    OpenGLShape(p1, p2, p3){
    reCalculateVertices();
}

void Sphere::reCalculateVertices(){
    if (!m_needRecalculate) return;

    m_coords.clear();

    std::vector<OpenGLVertex> vertices;
    ShapeUtil shapeutil;
    // Spherical coordinates
    // phi is the azimuthal angle, theta is the polar angle
    // phi ranges from 0 to 2 * PI, stepSize is 2 * PI / max(3, m_p2)
    // theta ranges from 0 to PI, stepSize is PI / max(2, m_p1)
    // If we define a point by (phi, theta), its coordinate can be
    // computed as thus:
    // x = r * sin(theta) * cos(phi)
    // y = r * sin(theta) * sin(phi)
    // z = r * cos(theta)
    int p1 = glm::max(2, m_p1), p2 = glm::max(3, m_p2);

    vertices.reserve((2 + p1) * 2 * p2);

    // Build a spherical strip
    std::vector<OpenGLVertex> side;
    glm::vec4 A = glm::vec4(0, 0, m_radius, 1);
    glm::vec4 B = glm::vec4(0, 0, -m_radius, 1);

    for (int i = 0; i < p2; i++) {
        shapeutil.buildSphericalStripUV2(side, A, B, p1, p2, i);
        int sz = side.size();
        if (i > 0)
            vertices.push_back(side[0]);
        for (int j = 0; j < sz; j++)
            vertices.push_back(side[j]);
        if (i < p2  - 1)
            vertices.push_back(side[side.size() - 1]);
        side.clear();
    }

    // 3. Populate m_coords
    populateCoordinatesUV(vertices);
} // Sphere::reCalculateVertices
