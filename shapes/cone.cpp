#include "cone.h"
#include "glm.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "shape2d.h"

Cone::Cone(int p1, int p2, int p3):
    OpenGLShape(p1, p2, p3)
{
    reCalculateVertices();
}

void Cone::reCalculateVertices() {
    if (!m_needRecalculate) return;

        m_coords.clear();

        std::vector<glm::vec4> vertices;

        // 1. Build the bottom
        Shape2D shape2d_helper;
        shape2d_helper.buildCircle(vertices, m_p2, m_p1, m_radius);

        // 2. Build the side
        // 2.1 Build a single side
        std::vector<glm::vec4> side;    // Single side with the following vertices: A = (0, 0.5, 0), B = (cos(0), -0.5, sin(0)), C = (cos(0 + delta), -0.5, sin(0 + delta))
        float theta = PI / 2.0f, delta = 2 * PI / m_p2;
        glm::vec4 A(0, m_radius, 0, 1);
        glm::vec4 B(m_radius * glm::cos(theta), -m_radius, m_radius * glm::sin(theta), 1);
        glm::vec4 C(m_radius * glm::cos(theta + delta), -m_radius, m_radius * glm::sin(theta + delta), 1);
        shape2d_helper.buildTriangleStrip(side, A, C, B, m_p1);

        // 2.2 Rotate and duplicate the side
        for (int i = 0; i < m_p2; i++) {
            glm::mat4 rot = glm::rotate(i * delta, glm::vec3(0, 1, 0));
            for (int j = 0; j < side.size(); j++)
                vertices.push_back(rot * side[j]);
        }

        float* data;
        for (glm::vec4& v : vertices) {
            data = glm::value_ptr(v);
            m_coords.push_back(*data);
            m_coords.push_back(*(data + 1));
            m_coords.push_back(*(data + 2));
            m_coords.push_back(0.0f);
            m_coords.push_back(0.0f);
            m_coords.push_back(-1.0f);
        }

        static constexpr int kFloatsPerVertex = 6;
        m_numVertices = m_coords.size() / kFloatsPerVertex;
        setVertexData(m_coords.data(), m_coords.size(), VBO::GEOMETRY_LAYOUT::LAYOUT_TRIANGLE_STRIP, m_numVertices);
        setAttribute(ShaderAttrib::POSITION, 3, 0, VBOAttribMarker::DATA_TYPE::FLOAT, false);
        setAttribute(ShaderAttrib::NORMAL, 3, 12, VBOAttribMarker::DATA_TYPE::FLOAT, false);
        buildVAO();

        m_needRecalculate = false;
}
