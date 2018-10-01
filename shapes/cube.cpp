#include "cube.h"
#include "glm.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "shapeutil.h"

Cube::Cube(int p1, int p2, float p3) :
    OpenGLShape(p1, p2, p3){
    reCalculateVertices();
}

void Cube::reCalculateVertices(){
    if (!m_needRecalculate) return;

    m_coords.clear();
    ShapeUtil shapeutil;
    std::vector<glm::vec4> vertices;
    std::vector<glm::vec4> zside0; // z = -m_radius face
    float stepSize = 2.0f * m_radius / m_p1;

    for (int i = 0; i < m_p1; i++) {
        // Set up the vertex for four corners
        glm::vec4 A = glm::vec4(-m_radius, m_radius - i * stepSize, m_radius, 1);
        glm::vec4 B = glm::vec4(-m_radius, m_radius - (i + 1) * stepSize, m_radius, 1);
        glm::vec4 C = glm::vec4(m_radius, m_radius - i * stepSize, m_radius, 1);
        glm::vec4 D = glm::vec4(m_radius, m_radius - (i + 1) * stepSize, m_radius, 1);
        shapeutil.buildQuadStrip(zside0, A, B, C, D, m_p1);
    }
    zside0.erase(zside0.end() - 2, zside0.end()); // Remove last one vertex and its normal, since it's of no use for connecting one face to another

    // Identity
    glm::mat4x4 z0z0 = glm::rotate(0.0f, glm::vec3(0, 1, 0));

    // Rotate zside0 to zside1
    glm::mat4x4 z0z1 = glm::rotate(PI, glm::vec3(0, 1, 0));

    // Rotate zside0 to xside0
    glm::mat4x4 z0x0 = glm::rotate(PI / 2.f, glm::vec3(0, 1, 0));

    // Rotate zside0 to xside0
    glm::mat4x4 z0x1 = glm::rotate(-PI / 2.f, glm::vec3(0, 1, 0));

    // Rotate zside0 to yside0
    glm::mat4x4 z0y0 = glm::rotate(PI / 2.f, glm::vec3(1, 0, 0));

    // Rotate zside0 to yside1
    glm::mat4x4 z0y1 = glm::rotate(-PI / 2.f, glm::vec3(1, 0, 0));


    bool is_z0 = true;
    std::vector<glm::mat4x4 *> rot_ptrs = { &z0z0, &z0z1, &z0x0, &z0x1, &z0y0, &z0y1 };
    for (glm::mat4x4 *& rot_ptr: rot_ptrs) {
        glm::mat4x4 rot = *rot_ptr;
        // Degenerate triangles
        if (!is_z0) {
            vertices.push_back(rot * zside0[0]);
            vertices.push_back(rot * zside0[1]);
        } else {
            is_z0 = false;
        }
        for (glm::vec4& v : zside0) {
            vertices.push_back(rot * v);
        }
    }
    populateCoordinates(vertices);
} // Cube::reCalculateVertices
