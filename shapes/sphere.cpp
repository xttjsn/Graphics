#include "sphere.h"
#include "glm.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "shape2d.h"

Sphere::Sphere(int p1, int p2, int p3):
    OpenGLShape(p1, p2, p3)
{
    reCalculateVertices();
}

void Sphere::reCalculateVertices() {
    if (!m_needRecalculate) return;

    m_coords.clear();

    std::vector<glm::vec4> vertices;

    // Spherical coordinates
    // phi is the azimuthal angle, theta is the polar angle
    // phi ranges from 0 to 2 * PI, stepSize is 2 * PI / max(3, m_p2)
    // theta ranges from 0 to PI, stepSize is PI / max(2, m_p1)
    // If we define a point by (phi, theta), its coordinate can be
    // computed as thus:
    // x = r * sin(theta) * cos(phi)
    // y = r * sin(theta) * sin(phi)
    // z = r * cos(theta)

    Shape2D shape2d_helper;
    int p1 = glm::max(2, m_p1), p2 = glm::max(3, m_p2);
    float delta = 2 * PI / p2;

    // 1. Build a spherical strip
    std::vector<glm::vec4> side;
    glm::vec4 A = glm::vec4(0, 0, m_radius, 1);
    glm::vec4 B = glm::vec4(0, 0, -m_radius, 1);
    shape2d_helper.buildSphericalStrip(side, A, B, p1, p2);

    // 2. Rotate and duplicate the strip
    glm::mat4 rot;
    for (int i = 0; i < p2; i++) {
        rot = glm::rotate(delta * i, glm::vec3(0, 0, 1));
        if (i > 0) {
            vertices.push_back(rot * side[0]);
            vertices.push_back(rot * side[1]);
        }
        for (int j = 0; j < side.size(); j++) {
            vertices.push_back(rot * side[j]);
        }
        if (i < p2 - 1) {
            vertices.push_back(rot * side[side.size() - 2]);
            vertices.push_back(rot * side[side.size() - 1]);
        }
    }

    // 3. Populate m_coords
    float* data;
    for (glm::vec4& v : vertices) {
        data = glm::value_ptr(v);
        m_coords.push_back(*data);
        m_coords.push_back(*(data + 1));
        m_coords.push_back(*(data + 2));
    }

    static constexpr int kFloatsPerVertex = 6;
    m_numVertices = m_coords.size() / kFloatsPerVertex;
    setVertexData(m_coords.data(), m_coords.size(), VBO::GEOMETRY_LAYOUT::LAYOUT_TRIANGLE_STRIP, m_numVertices);
    setAttribute(ShaderAttrib::POSITION, 3, 0, VBOAttribMarker::DATA_TYPE::FLOAT, false);
    setAttribute(ShaderAttrib::NORMAL, 3, 12, VBOAttribMarker::DATA_TYPE::FLOAT, false);
    buildVAO();

    m_needRecalculate = false;
}
