#include "cone.h"
#include "glm.hpp"
#include "glm/gtx/transform.hpp"
#include "shapeutil.h"

Cone::Cone(int p1, int p2, float p3) :
    OpenGLShape(p1, p2, p3){
    reCalculateVertices();
}

void Cone::reCalculateVertices() {
    if (!m_needRecalculate) return;

    m_coords.clear();
    std::vector<OpenGLVertex> vertices;
    ShapeUtil shapeutil;
    int p1 = m_p1, p2 = glm::max(3, m_p2);

    vertices.reserve((1 + 2 * p1 + 2) * p2 * 2);

    // 1. Build the bottom
    shapeutil.buildCircleUV(vertices, p2, p1, m_radius);

    // 2. Build the side
    std::vector<OpenGLVertex> side;
    std::vector<OpenGLVertex> side_next;
    float theta = 0.0f, delta = 2 * PI / p2;

    glm::vec4 A, B, C;
    glm::mat4x4 rot_next = glm::rotate(delta, glm::vec3(0, 1, 0));
    glm::mat4x4 rot_prev = glm::rotate(-delta, glm::vec3(0, 1, 0));
    for (int i = 0; i < p2; i++) {
        A = glm::vec4(0, m_radius, 0, 1);
        B = glm::vec4(m_radius * glm::cos(theta + i * delta), -m_radius, m_radius * glm::sin(theta + i * delta), 1);
        C = glm::vec4(m_radius * glm::cos(theta + (i + 1) * delta), -m_radius, m_radius * glm::sin(theta + (i + 1) * delta), 1);
        shapeutil.buildTriangleStripUV(side, A, C, B, p1, p2, i);

        for (unsigned int j = 0; j < side.size(); j++) {
            if (j == 0 || j == side.size() - 1)
                vertices.push_back(side[j]);
            else if (j % 2 == 0)
                vertices.push_back(side[j].normAvgWithRotation(rot_next));
            else
                vertices.push_back(side[j].normAvgWithRotation(rot_prev));
        }
        side.clear();
    }

    populateCoordinatesUV(vertices);
}

//void Cone::reCalculateVertices(){
//    if (!m_needRecalculate) return;

//    m_coords.clear();
//    std::vector<glm::vec4> vertices;
//    ShapeUtil shapeutil;
//    int p1 = m_p1, p2 = glm::max(3, m_p2);

//    vertices.reserve((1 + 2 * p1 + 2) * p2 * 2);

//    // 1. Build the bottom
//    shapeutil.buildCircle(vertices, p2, p1, m_radius);

//    // 2. Build the side
//    // 2.1 Build a single side
//    std::vector<glm::vec4> side; // Single side with the following vertices: A = (0, 0.5, 0), B = (cos(0), -0.5, sin(0)), C = (cos(0 + delta), -0.5, sin(0 + delta))
//    float theta = PI / 2.0f, delta = 2 * PI / p2;
//    glm::vec4 A(0, m_radius, 0, 1);
//    glm::vec4 B(m_radius * glm::cos(theta), -m_radius, m_radius * glm::sin(theta), 1);
//    glm::vec4 C(m_radius * glm::cos(theta + delta), -m_radius, m_radius * glm::sin(theta + delta), 1);
//    shapeutil.buildTriangleStrip(side, A, C, B, p1);

//    // 2.2 Rotate and duplicate the side
//    glm::vec4 norm;
//    int sz = side.size();
//    for (int i = 0; i < p2; i++) {
//        glm::mat4 rot      = glm::rotate(i * delta, glm::vec3(0, 1, 0));
//        glm::mat4 rot_prev = glm::rotate((i - 1) * delta, glm::vec3(0, 1, 0));
//        glm::mat4 rot_next = glm::rotate((i + 1) * delta, glm::vec3(0, 1, 0));
//        for (int j = 0; j < sz; j += 2) {   // Use static_cast to avoid -Wsign-compare
//            vertices.push_back(rot * side[j]);
//            if (j < 2 || j == sz - 2)
//                vertices.push_back(rot * side[j + 1]);
//            else if (j % 4 == 0)
//                vertices.push_back(glm::normalize(shapeutil.interpolate(rot * side[j + 1], rot_next * side[j + 1],
//                  0.5f)));
//            else
//                vertices.push_back(glm::normalize(shapeutil.interpolate(rot * side[j + 1], rot_prev * side[j + 1],
//                  0.5f)));
//        }
//    }


//    populateCoordinates(vertices);
//} // Cone::reCalculateVertices
