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

    std::vector<OpenGLVertex> vertices;
    std::vector<OpenGLVertex> zside0;
    float stepSize = 2.0f * m_radius / m_p1;
    vertices.reserve(2 * (m_p1 + 2) * m_p1 * 6);

    for (int i = 0; i < m_p1; i++) {
        glm::vec4 Apos = glm::vec4(-m_radius, m_radius - i * stepSize, m_radius, 1);
        glm::vec2 Auv  = glm::vec2(0, i / static_cast<float>(m_p1));

        glm::vec4 Bpos = glm::vec4(-m_radius, m_radius - (i + 1) * stepSize, m_radius, 1);
        glm::vec2 Buv  = glm::vec2(0, (i + 1) / static_cast<float>(m_p1));

        glm::vec4 Cpos = glm::vec4(m_radius, m_radius - i * stepSize, m_radius, 1);
        glm::vec2 Cuv  = glm::vec2(1, i / static_cast<float>(m_p1));

        glm::vec4 Dpos = glm::vec4(m_radius, m_radius - (i + 1) * stepSize, m_radius, 1);
        glm::vec2 Duv  = glm::vec2(1, (i + 1) / static_cast<float>(m_p1));

        shapeutil.buildQuadStripUV(zside0, Apos, Bpos, Cpos, Dpos, Auv, Buv, Cuv, Duv, m_p1);
    }
    zside0.erase(zside0.end() - 1, zside0.end());

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

    bool is_z0                          = true;
    std::vector<glm::mat4x4 *> rot_ptrs = { &z0z0, &z0z1, &z0x0, &z0x1, &z0y0, &z0y1 };
    for (glm::mat4x4 *& rot_ptr: rot_ptrs) {
        glm::mat4x4 rot = *rot_ptr;
        // Degenerate triangles
        if (!is_z0) {
            vertices.push_back(zside0[0].rotate(rot));
        } else {
            is_z0 = false;
        }
        for (OpenGLVertex v : zside0) {
            vertices.push_back(v.rotate(rot));
        }
    }

    populateCoordinatesUV(vertices);
} // Cube::reCalculateVertices
