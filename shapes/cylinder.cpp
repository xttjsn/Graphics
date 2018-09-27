#include "cylinder.h"

Cylinder::Cylinder(int p1, int p2, int p3):
    OpenGLShape(p1, p2, p3)
{
    reCalculateVertices();
}

void Cylinder::reCalculateVertices() {
    if (!m_needRecalculate) return;

    m_coords.clear();


    // A cylinder has two

    static constexpr int kFloatsPerVertex = 6;
    m_numVertices = m_coords.size() / kFloatsPerVertex;
    setVertexData(m_coords.data(), m_coords.size(), VBO::GEOMETRY_LAYOUT::LAYOUT_TRIANGLE_STRIP, m_numVertices);
    setAttribute(ShaderAttrib::POSITION, 3, 0, VBOAttribMarker::DATA_TYPE::FLOAT, false);
    setAttribute(ShaderAttrib::NORMAL, 3, 12, VBOAttribMarker::DATA_TYPE::FLOAT, false);
    buildVAO();

    m_needRecalculate = false;
}
