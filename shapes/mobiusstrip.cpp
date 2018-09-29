#include "mobiusstrip.h"
#include "glm.hpp"
#include "glm/gtx/transform.hpp"
#include "shapeutil.h"

MobiusStrip::MobiusStrip(int p1, int p2, float p3) : OpenGLShape(p1, p2, p3){
    reCalculateVertices();
}

void MobiusStrip::reCalculateVertices(){
    if (!m_needRecalculate) return;

    m_coords.clear();

    std::vector<glm::vec4> vertices;

    /*
     * Mobius strip geometry
     *
     * x(u, v) = (1 + v/2 * cos(u / 2)) * cos(u)
     * y(u, v) = (1 + v/2 * cos(u / 2)) * sin(u)
     * z(u, v) = v/2 * sin(u / 2)
     * where 0 <= u < 2 * PI, and -1 <= v <= 1
     *
     * We let p1 be the number of steps in v space, and p2 be the number of steps in u space.
     * Firstly we fix u, then build a quad strip using varying v
     * Then we change u, then build the next quad strip, and so on.
     * Finally, we go back and build the entire mobius strip again in clock-wise order, so that
     * both sides in a quad can be rendered and have normals.
     *
     * The normal of each vertex is the averaged normal of the two adjacent quads / triangles.
     */
    int p1 = m_p1, p2 = glm::max(3, m_p2);

    ShapeUtil shapeutil;
    shapeutil.buildMobiusStrip(vertices, p1, p2, true);
    shapeutil.buildMobiusStrip(vertices, p1, p2, false);

    // Populate m_coords
    populateCoordinates(vertices);
} // MobiusStrip::reCalculateVertices
