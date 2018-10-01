#include "cylinder.h"
#include "glm.hpp"
#include "glm/gtx/transform.hpp"
#include "shapeutil.h"

Cylinder::Cylinder(int p1, int p2, float p3) :
    OpenGLShape(p1, p2, p3){
    reCalculateVertices();
}

void Cylinder::reCalculateVertices(){
    if (!m_needRecalculate) return;

    m_coords.clear();

    std::vector<glm::vec4> vertices;

    ShapeUtil shapeutil;

    int p2 = glm::max(m_p2, 3), p1 = m_p1;

    // Build bottom circle
    shapeutil.buildCircle(vertices, p2, p1, m_radius);
    vertices.erase(vertices.end() - 2, vertices.end());

    // Remove the reduant center vetex and norm,
    // replace it with the last vertex and last norm
    glm::vec4 last_vert = vertices[vertices.size() - 2];
    glm::vec4 last_norm = vertices[vertices.size() - 1];
    vertices.push_back(last_vert);
    vertices.push_back(last_norm);

    // Build top circle
    glm::mat4 rot       = glm::rotate(PI, glm::vec3(0, 0, 1));
    int sz              = vertices.size();
    for (int i = 0; i < sz; i++) {
        vertices.push_back(rot * vertices[i]);
    }

    // Build one side
    std::vector<glm::vec4> side;
    float delta = 2 * PI / p2;
    glm::vec4 A = glm::vec4(0, m_radius, m_radius, 1);
    glm::vec4 B = glm::vec4(-m_radius * glm::sin(delta), m_radius, m_radius * glm::cos(delta), 1);
    glm::vec4 C = glm::vec4(0, -m_radius, m_radius, 1);
    glm::vec4 D = glm::vec4(-m_radius * glm::sin(delta), -m_radius, m_radius * glm::cos(delta), 1);
    shapeutil.buildQuadStrip(side, B, A, D, C, p1);

    // Duplicate for all sides
    for (int i = 0; i < p2; ++i) {
        rot = glm::rotate(delta * i, glm::vec3(0, 1, 0)); // Rotate around y axis

        // Two normals for adjacent sides`
        glm::vec4 norm_even = glm::vec4(glm::sin(delta * i), 0, glm::cos(delta * i), 0);
        glm::vec4 norm_odd  = glm::vec4(glm::sin(delta * (i - 1)), 0, glm::cos(delta * (i - 1)), 0);

        // Degenerate triangles
        if (i == 0) {
            vertices.push_back(rot * side[0]);
            vertices.push_back(norm_odd);
        }
        int sz = side.size();
        for (int j = 0; j < sz; j += 4) {
            vertices.push_back(rot * side[j]); // vertex
            vertices.push_back(j == sz - 4 ? norm_even : norm_odd);
            vertices.push_back(rot * side[j + 2]);
            vertices.push_back(glm::vec4(0, 0, 0, 0)); // No need for extra normal
        }
    }

    populateCoordinates(vertices);
} // Cylinder::reCalculateVertices
