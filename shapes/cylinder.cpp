#include "cylinder.h"
#include "glm.hpp"
#include "glm/gtx/transform.hpp"
#include "shapeutil.h"

Cylinder::Cylinder(int p1, int p2, float p3) :
    OpenGLShape(p1, p2, p3){
    reCalculateVertices();
}

void Cylinder::reCalculateVertices() {
    if (!m_needRecalculate) return;

    m_coords.clear();

    std::vector<OpenGLVertex> vertices;

    ShapeUtil shapeutil;

    int p2 = glm::max(m_p2, 3), p1 = m_p1;

    vertices.reserve((1 + 2 * p1 + 2) * p2 * 2 + (2 * (p1 + 2) * 2));

    // Build bottom circle
    shapeutil.buildCircleUV(vertices, p2, p1, m_radius);

    // Remove the redudant center vertex
    // Replace it with the last vertex
    vertices.erase(vertices.end() - 1, vertices.end());
    vertices.push_back(vertices[vertices.size() - 1]);

    // Build top cicle
    glm::mat4 rot       = glm::rotate(PI, glm::vec3(0, 0, 1));
    int sz              = vertices.size();
    for (int i = 0; i < sz; i++) {
        vertices.push_back(vertices[i].rotate(rot));
        vertices[vertices.size() - 1].texcoord.y = 1.0f - vertices[vertices.size() - 1].texcoord.y;
        vertices[vertices.size() - 1].texcoord.x = 1.0f - vertices[vertices.size() - 1].texcoord.x;
    }

    // Prepare parameters
    std::vector<OpenGLVertex> side;
    float theta = PI / 2.0f, delta = 2.0f * PI / p2;
    glm::vec4 A, B, C, D;
    glm::mat4 rot_next = glm::rotate(delta, glm::vec3(0, 1, 0));
    glm::mat4 rot_prev = glm::rotate(-delta, glm::vec3(0, 1, 0));

    // Build all sides
    for (int i = 0; i < p2; ++i) {
        A = glm::vec4(m_radius * glm::sin(theta + i * delta), m_radius, m_radius * glm::cos(theta + i * delta), 1);
        B = glm::vec4(m_radius * glm::sin(theta + (i + 1) * delta), m_radius, m_radius * glm::cos(theta + (i + 1) * delta), 1);
        C = glm::vec4(m_radius * glm::sin(theta + i * delta), -m_radius, m_radius * glm::cos(theta + i * delta), 1);
        D = glm::vec4(m_radius * glm::sin(theta + (i + 1) * delta), -m_radius, m_radius * glm::cos(theta + (i + 1) * delta), 1);

        shapeutil.buildQuadStripUV(side, A, B, C, D, p1, p2, i);

        // Degenerate triangles
        if (i == 0) {
            vertices.push_back(side[0].normAvgWithRotation(rot_prev));
        }

        int sz = side.size();
        for (int j = 0; j < sz; j++) {
            if (j >= sz - 2)
                vertices.push_back(side[j].normAvgWithRotation(rot_next));
            else
                vertices.push_back(side[j].normAvgWithRotation(j % 2 == 0 ? rot_prev : rot_next));
        }

        side.clear();
    }

    populateCoordinatesUV(vertices);

}
