#include "torus.h"
#include "glm.hpp"
#include "glm/gtx/transform.hpp"
#include "shapeutil.h"

Torus::Torus(int p1, int p2, float p3) :
    OpenGLShape(p1, p2, p3){
    reCalculateVertices();
}

void Torus::reCalculateVertices(){
    if (!m_needRecalculate) return;

    m_coords.clear();
    std::vector<glm::vec4> vertices;
    ShapeUtil shapeutil;

    /*
     * To build torus, we first build two circle of vertices, which are then
     * connected in a zig-zag way to form a segment of the torus.
     * Then we duplicate that segment to build the entire torus.
     * p1: number of segments in the torus
     * p2: number of sides of each segments
     * p3: radius of segment cross section
     * delta: radian degreen between two adjacent segment circle's centers
     */
    int p1 = glm::max(3, m_p1), p2 = glm::max(3, m_p2);
    float delta = 2.0f * PI / p1, radius = m_radius, p3 = m_p3 / 100.0f * m_radius;

    // 1. Set up two centers of those two circles
    glm::vec4 center1 = glm::vec4(0, radius - p3, 0, 1);
    glm::vec4 center2 = glm::vec4((radius - p3) * glm::cos(PI / 2 + delta),
        (radius - p3) * glm::sin(PI / 2 + delta), 0, 1);

    // 2. Build two circles of vertices with radius p3, with a specified phi
    std::vector<glm::vec4> circle1, circle2;
    shapeutil.buildCircleOfVertices(circle1, center1, p3, p2, PI / 2);
    shapeutil.buildCircleOfVertices(circle2, center2, p3, p2, PI / 2 + delta);

    // 3. Connect two circles of vertices and get the segment
    std::vector<glm::vec4> segment;
    shapeutil.buildSegmentFromCircles(segment, circle1, circle2);

    // 4. Duplicate segements to form the torus
    glm::mat4 rot;
    int sz = segment.size();
    for (int i = 0; i < p1; i++) {
        rot = glm::rotate(i * delta, glm::vec3(0, 0, 1));
        for (int j = 0; j < sz; j++) {
            vertices.push_back(rot * segment[j]);
        }
    }

    // 5. Populate m_coords
    populateCoordinates(vertices);
} // Torus::reCalculateVertices
