#include "cube.h"
#include "glm.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "shape2d.h"
#include <tuple>

Cube::Cube(int p1, int p2, int p3) :
        OpenGLShape(p1, p2, p3)
{
        reCalculateVertices();
}

void Cube::reCalculateVertices() {
        if (!m_needRecalculate) return;

        m_coords.clear();

        std::vector<glm::vec4> zside0;
        std::vector<glm::vec4> zside1;
        std::vector<glm::vec4> yside0;
        std::vector<glm::vec4> yside1;
        std::vector<glm::vec4> xside0;
        std::vector<glm::vec4> xside1;

        float stepSize = 2.0f * m_radius / m_p1, x, y, z;

        // z = -0.5 face
        Shape2D shape2D_helper;

        for (int i = 0; i < m_p1; i++) {
            // Set up the vertex for four corners
            glm::vec4 A = glm::vec4(-m_radius, m_radius - i * stepSize, m_radius, 1);
            glm::vec4 B = glm::vec4(-m_radius, m_radius - (i + 1) * stepSize, m_radius, 1);
            glm::vec4 C = glm::vec4(m_radius, m_radius - i * stepSize, m_radius, 1);
            glm::vec4 D = glm::vec4(m_radius, m_radius - (i + 1) * stepSize, m_radius, 1);
            shape2D_helper.buildQuadStrip(zside0, A, B, C, D, m_p1);
        }
        zside0.erase(zside0.end() - 2, zside0.end());  // Remove last one vertex and its normal, since it's of no use for connecting one face to another

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

        // Degenerate triangles connecting face to face
        zside1.push_back(z0z1 * zside0[0]);
        zside1.push_back(z0z1 * zside0[1]);

        xside0.push_back(z0x0 * zside0[0]);
        xside0.push_back(z0x0 * zside0[1]);

        xside1.push_back(z0x1 * zside0[0]);
        xside1.push_back(z0x1 * zside0[1]);

        yside0.push_back(z0y0 * zside0[0]);
        yside0.push_back(z0y0 * zside0[1]);

        yside1.push_back(z0y1 * zside0[0]);
        yside1.push_back(z0y1 * zside0[1]);

        for (glm::vec4& v : zside0) {
                zside1.push_back(z0z1 * v);
                xside0.push_back(z0x0 * v);
                xside1.push_back(z0x1 * v);
                yside0.push_back(z0y0 * v);
                yside1.push_back(z0y1 * v);
        }

        float* data;
        std::vector<glm::vec4>* side_ptrs[] = {&zside0, &zside1, &xside0, &xside1, &yside0, &yside1};
        for (std::vector<glm::vec4>* side_ptr : side_ptrs) {
            std::vector<glm::vec4>& side = *side_ptr;
            for (int i = 0; i < side.size(); ++i) {
                data = glm::value_ptr(side[i]);
                m_coords.push_back(*(data));
                m_coords.push_back(*(data + 1));
                m_coords.push_back(*(data + 2));
            }
        }

        static constexpr int kFloatsPerVertex = 6;
        m_numVertices = m_coords.size() / kFloatsPerVertex;
        setVertexData(m_coords.data(), m_coords.size(), VBO::GEOMETRY_LAYOUT::LAYOUT_TRIANGLE_STRIP, m_numVertices);
        setAttribute(ShaderAttrib::POSITION, 3, 0, VBOAttribMarker::DATA_TYPE::FLOAT, false);
        setAttribute(ShaderAttrib::NORMAL, 3, 12, VBOAttribMarker::DATA_TYPE::FLOAT, false);
        buildVAO();

        m_needRecalculate = false;
}
