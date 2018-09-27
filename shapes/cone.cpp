#include "cone.h"
#include "glm.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/type_ptr.hpp"

Cone::Cone(int p1, int p2, int p3):
    OpenGLShape(p1, p2, p3)
{
    reCalculateVertices();
}

void Cone::reCalculateVertices() {
    if (!m_needRecalculate) return;

        m_coords.clear();

        // 1. Build the bottom
        // Start with (0, -0.5, 0)
        // m_p1 is the number of sides, minimum is 3
        // m_p2 is the number of stacks, minimum is 1
        int p1 = glm::max(m_p1, 3);
        float delta = 2.0f * PI / p1, theta = 0.0f, delta_rad = m_radius / m_p2, rad = 0.0f, x, y, z;

        std::vector<glm::vec4> slice;
        x = 0.0f, y = -m_radius, z = 0.0f;
        for (int i = 0; i <= 2 * p1; ++i) {
            theta = i * delta;
            slice.push_back(glm::vec4(x, y, z, 1));
            for (int j = 1; j <= m_p2; ++j) {
                rad = delta_rad * j;
                slice.push_back(glm::vec4(x + rad * glm::cos(theta), y + rad * glm::sin(theta), z, 1));
                slice.push_back(glm::vec4(x + rad * glm::cos(theta + delta), y + rad * glm::sin(theta + delta), z, 1));
            }
            // Degenerate triangles ?
        }


        //

        float* data;
        for (glm::vec4& v : slice) {
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
