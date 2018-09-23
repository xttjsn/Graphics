#include "cube.h"
#include "glm.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/type_ptr.hpp"
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

        float stepSize = 1.0f / m_p1, x, y, z;

        // z = -0.5 face
        x = -m_radius, z = y = m_radius;
        for (int j = 0; j < m_p1; ++j) {
                y -= j * stepSize;
                for (int i = 0; i <= m_p1; ++i) {
                        x += i * stepSize;
                        zside0.push_back(glm::vec4(x, y, z, 1));
                        zside0.push_back(glm::vec4(x, y - stepSize, z, 1));
                        x -= i * stepSize;
                }
                // For degenerate triangles
                zside0.push_back(glm::vec4(x + stepSize * m_p1, y - stepSize, z, 1));
                zside0.push_back(glm::vec4(x, y - stepSize, z, 1));
                y += j * stepSize;
        }
        zside0.erase(zside0.end() - 1); // Remove the last element, since the next point will be from the next surface

        // Translate zside0 to zside1
        glm::mat4x4 z0z1 = glm::rotate(PI, glm::vec3(0, 1, 0));

        // Move zside0 to xside0
        glm::mat4x4 z0x0 = glm::rotate(PI / 2.f, glm::vec3(0, 1, 0));

        // Move zside0 to xside0
        glm::mat4x4 z0x1 = glm::rotate(-PI / 2.f, glm::vec3(0, 1, 0));

        // Move zside0 to yside0
        glm::mat4x4 z0y0 = glm::rotate(PI / 2.f, glm::vec3(1, 0, 0));

        // Move zside0 to yside1
        glm::mat4x4 z0y1 = glm::rotate(-PI / 2.f, glm::vec3(1, 0, 0));

        zside1.push_back(z0z1 * zside0[0]);
        xside0.push_back(z0x0 * zside0[0]);
        xside1.push_back(z0x1 * zside0[0]);
        yside0.push_back(z0y0 * zside0[0]);
        yside1.push_back(z0y1 * zside0[0]);
        for (glm::vec4& v : zside0) {
                zside1.push_back(z0z1 * v);
                xside0.push_back(z0x0 * v);
                xside1.push_back(z0x1 * v);
                yside0.push_back(z0y0 * v);
                yside1.push_back(z0y1 * v);
        }
        yside1.erase(yside1.end() - 1);

        float* data;
        std::vector<glm::vec4>* side_ptrs[] = {&zside0, &zside1, &xside0, &xside1, &yside0, &yside1};
        std::vector<float> norm_z0 = {0.0f, 0.0f, 1.0f}, norm_z1 = {0.0f, 0.0f, -1.0f},
                           norm_x0 = {1.0f, 0.0f, 0.0f}, norm_x1 = {-1.0f, 0.0f, 0.0f},
                           norm_y0 = {0.0f, -1.0f, 0.0f}, norm_y1 = {0.0f, 1.0f, 0.0f};
        std::vector<float>* norm_ptrs[] = {&norm_z0, &norm_z1, &norm_x0, &norm_x1, &norm_y0, &norm_y1};
        int norm_idx = 0;
        for (std::vector<glm::vec4>* side_ptr : side_ptrs) {
                std::vector<glm::vec4>& side = *side_ptr;
                for (int i = 0; i < side.size(); ++i) {
                        data = glm::value_ptr(side[i]);
                        m_coords.push_back(*data);
                        m_coords.push_back(*(data + 1));
                        m_coords.push_back(*(data + 2));
                        m_coords.push_back(norm_ptrs[norm_idx]->at(0));
                        m_coords.push_back(norm_ptrs[norm_idx]->at(1));
                        m_coords.push_back(norm_ptrs[norm_idx]->at(2));
                }
                norm_idx += 1;
        }

        static constexpr int kFloatsPerVertex = 6;
        // 1 -> 2 * 2 * 6
        // 2 -> 3 * 2 * 6
        // 3 -> (4 * 2 * 3 + 2 * 3) * 6
        // 4 -> 5 * 2 * 4 * 6
        // n -> ((n + 1) * 2 + 2) * n * 6
        m_numVertices = m_coords.size() / kFloatsPerVertex;
        setVertexData(m_coords.data(), m_coords.size(), VBO::GEOMETRY_LAYOUT::LAYOUT_TRIANGLE_STRIP, m_numVertices);
        setAttribute(ShaderAttrib::POSITION, 3, 0, VBOAttribMarker::DATA_TYPE::FLOAT, false);
        setAttribute(ShaderAttrib::NORMAL, 3, 12, VBOAttribMarker::DATA_TYPE::FLOAT, false);
        buildVAO();

        m_needRecalculate = false;
}
