#include "cylinder.h"
#include "glm.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "shapeutil.h"

Cylinder::Cylinder(int p1, int p2, int p3):
    OpenGLShape(p1, p2, p3)
{
    reCalculateVertices();
}

void Cylinder::reCalculateVertices() {
    if (!m_needRecalculate) return;

    m_coords.clear();

    std::vector<glm::vec4> vertices;

    ShapeUtil shapeutil_helper;

    int p2 = glm::max(m_p2, 3), p1 = m_p1;

    // Build bottom circle
    shapeutil_helper.buildCircle(vertices, p2, p1, m_radius);

    // Build top circle
    glm::mat4 rot = glm::rotate(PI, glm::vec3(0, 0, 1));
    int sz = vertices.size();
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
    shapeutil_helper.buildQuadStrip(side, A, B, C, D, p1);

    // Duplicate for all sides
    for (int i = 0; i < p2; ++i) {
        rot = glm::rotate(-delta * i, glm::vec3(0, 1, 0));  // Rotate around y axis
        for (glm::vec4& v : side) {
            vertices.push_back(rot * v);
        }
    }

    float* data;
    for (glm::vec4& v : vertices) {
        data = glm::value_ptr(v);
        m_coords.push_back(*(data));
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
