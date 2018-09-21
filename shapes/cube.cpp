#include "cube.h"
#include "glm.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/type_ptr.hpp"

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
     x = y = z = -m_radius;
     for (int i = 0; i < m_p1; ++i) {
         x += i * stepSize;
         for (int j = 0; j < m_p1; ++j) {
             y += j * stepSize;
             /* Left up and right down triangles */
             float vetices[] = {x, y, z, x, y + stepSize, z, x + stepSize, y + stepSize, z,
                               x, y, z, x + stepSize, y + stepSize, z, x + stepSize, y, z};
             for (int l = 0; l < 18; l+=3) {
                 zside0.push_back(glm::vec4(vetices[l], vetices[l+1], vetices[l+2], 1));
             }
             y -= j * stepSize;
         }
         x -= i * stepSize;
     }

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

     for (glm::vec4& v : zside0) {
        zside1.push_back(z0z1 * v);
        xside0.push_back(z0x0 * v);
        xside1.push_back(z0x1 * v);
        yside0.push_back(z0y0 * v);
        yside1.push_back(z0y1 * v);
     }

     float* data;
     std::vector<glm::vec4>* side_ptrs[] = {&zside0, &zside1, &yside0, &yside1, &xside0, &xside1};
     for (std::vector<glm::vec4>* side_ptr : side_ptrs) {
        std::vector<glm::vec4>& side = *side_ptr;
        for (int i = 0; i < side.size(); ++i) {
           data = glm::value_ptr(side[i]);
           m_coords.push_back(*data);
           m_coords.push_back(*(data + 1));
           m_coords.push_back(*(data + 2));
        }
     }

     m_numVertices = m_p1 * m_p1 * 36;
     setVertexData(m_coords.data(), m_coords.size(), VBO::GEOMETRY_LAYOUT::LAYOUT_TRIANGLES, m_numVertices);
     setAttribute(ShaderAttrib::POSITION, 3, 0, VBOAttribMarker::DATA_TYPE::FLOAT, false);
     buildVAO();

     m_needRecalculate = false;
}
