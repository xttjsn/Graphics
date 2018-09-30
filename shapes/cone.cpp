#include "cone.h"
#include "glm.hpp"
#include "glm/gtx/transform.hpp"
#include "shapeutil.h"

Cone::Cone(int p1, int p2, float p3) :
    OpenGLShape(p1, p2, p3){
    reCalculateVertices();
}

void Cone::reCalculateVertices(){
    if (!m_needRecalculate) return;

    m_coords.clear();
    std::vector<glm::vec4> vertices;
    ShapeUtil shapeutil;
    int p1 = m_p1, p2 = glm::max(3, m_p2);

    // 1. Build the bottom
    shapeutil.buildCircle(vertices, p2, p1, m_radius);

    // 2. Build the side
    // 2.1 Build a single side
    std::vector<glm::vec4> side; // Single side with the following vertices: A = (0, 0.5, 0), B = (cos(0), -0.5, sin(0)), C = (cos(0 + delta), -0.5, sin(0 + delta))
    float theta = PI / 2.0f, delta = 2 * PI / p2;
    glm::vec4 A(0, m_radius, 0, 1);
    glm::vec4 B(m_radius * glm::cos(theta), -m_radius, m_radius * glm::sin(theta), 1);
    glm::vec4 C(m_radius * glm::cos(theta + delta), -m_radius, m_radius * glm::sin(theta + delta), 1);
    shapeutil.buildTriangleStrip(side, A, C, B, p1);

    // 2.2 Rotate and duplicate the side
    for (int i = 0; i < p2; i++) {
        glm::mat4 rot = glm::rotate(i * delta, glm::vec3(0, 1, 0));
        for (int j = 0; j < side.size(); j++)
            vertices.push_back(rot * side[j]);
    }

    populateCoordinates(vertices);
}
