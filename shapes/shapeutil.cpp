#include "shapeutil.h"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#define PI 3.1415926f

ShapeUtil::ShapeUtil()
{ }

void ShapeUtil::buildCircle(std::vector<glm::vec4> &data, int p1, int p2, float radius){
    // Start with (0, -0.5, 0)
    // m_p1 is the number of sides, minimum is 3

    p1 = glm::max(p1, 3);
    float x = 0.0f, y = -radius, z = 0.0f, delta = 2.0 * PI / p1, theta = PI / 2.0f;
    glm::vec4 A = glm::vec4(x, y, z, 1);
    glm::vec4 B = glm::vec4(x + radius * glm::cos(theta), y, z + radius * glm::sin(theta), 1);
    glm::vec4 C = glm::vec4(x + radius * glm::cos(theta + delta), y, z + radius * glm::sin(theta + delta), 1);

    std::vector<glm::vec4> slice;
    buildTriangleStrip(slice, A, B, C, p2);

    glm::mat4 rot;
    for (int i = 0; i < p1; ++i) {
        rot = glm::rotate(i * delta, glm::vec3(0, 1, 0));
        for (int j = 0; j < slice.size(); ++j) {
            data.push_back(rot * slice[j]);
        }
    }
}

void ShapeUtil::buildTriangleStrip(std::vector<glm::vec4>& data, glm::vec4 A, glm::vec4 B, glm::vec4 C, int numSlides){
    /*       A
     *      / \
     *     /   \
     *    B --- C
     */
    glm::vec3 norm3 = glm::normalize(glm::cross(glm::vec3(B - A), glm::vec3(C - A)));
    glm::vec4 norm4 = glm::vec4(norm3, 0);
    data.push_back(A);
    data.push_back(norm4);
    for (int t = 1; t <= numSlides; ++t) {
        glm::vec4 right = interpolate(A, B, t / static_cast<float>(numSlides));
        glm::vec4 left  = interpolate(A, C, t / static_cast<float>(numSlides));
        data.push_back(right);
        data.push_back(norm4);
        data.push_back(left);
        data.push_back(norm4);
    }
    data.push_back(A);
    data.push_back(norm4);
}

void ShapeUtil::buildQuadStrip(std::vector<glm::vec4>& data, glm::vec4 A, glm::vec4 B, glm::vec4 C, glm::vec4 D,
  int numQuads){
    /* A ---- C
     * |      |
     * |      |
     * B ---- D
     */
    glm::vec3 norm3 = glm::normalize(glm::cross(glm::vec3(A - C), glm::vec3(B - A)));
    glm::vec4 norm4 = glm::vec4(norm3, 0);
    for (int t = 0; t <= numQuads; t++) {
        glm::vec4 up   = interpolate(A, C, t / static_cast<float>(numQuads));
        glm::vec4 down = interpolate(B, D, t / static_cast<float>(numQuads));
        data.push_back(up);
        data.push_back(norm4);
        data.push_back(down);
        data.push_back(norm4);
    }
    data.push_back(D);
    data.push_back(norm4);
    data.push_back(B);
    data.push_back(norm4);
}

void ShapeUtil::buildSphericalStrip(std::vector<glm::vec4>& data, glm::vec4 A, glm::vec4 B, int numStacks,
  int numStrips){
    /*      A
     *     / \
     *    X---X
     *   /     \
     *  X-------X
     *   \     /
     *    X---X
     *     \ /
     *      B
     */
    glm::vec4 center = interpolate(A, B, 0.5);
    glm::vec3 norm3;
    glm::vec4 norm4;

    data.push_back(A);
    data.push_back(glm::normalize(A));

    float phi = 0.0f, theta = 0.0f, delta_phi = 2 * PI / numStrips, delta_theta = PI / numStacks,
      radius = glm::distance(A, B) / 2.0f;
    float x, y, z;
    for (int i = 1; i < numStacks; i++) {
        phi   = 0.0f;
        theta = i * delta_theta;
        x     = radius * glm::sin(theta) * glm::cos(phi);
        y     = radius * glm::sin(theta) * glm::sin(phi);
        z     = radius * glm::cos(theta);
        glm::vec4 left = glm::vec4(x, y, z, 1);

        phi = delta_phi;
        x   = radius * glm::sin(theta) * glm::cos(phi);
        y   = radius * glm::sin(theta) * glm::sin(phi);
        z   = radius * glm::cos(theta);
        glm::vec4 right = glm::vec4(x, y, z, 1);

        data.push_back(left);
        data.push_back(glm::normalize(left));
        data.push_back(right);
        data.push_back(glm::normalize(right));
    }

    data.push_back(B);
    data.push_back(glm::normalize(B));
} // ShapeUtil::buildSphericalStrip

void ShapeUtil::buildCircleOfVertices(std::vector<glm::vec4>& data, glm::vec4 center, float radius, int numPoints,
  float phi){
    float delta_theta = 2 * PI / numPoints, theta = 0.0f, x, y, z;

    glm::vec4 norm;

    for (int i = 0; i < numPoints; i++) {
        theta = i * delta_theta;
        x     = radius * glm::sin(theta) * glm::cos(phi);
        y     = radius * glm::sin(theta) * glm::sin(phi);
        z     = radius * glm::cos(theta);
        glm::vec4 point = glm::translate(glm::vec3(center)) * glm::vec4(x, y, z, 1);

        norm = glm::normalize(point - center);

        data.push_back(point);
        data.push_back(norm);
    }
}

void ShapeUtil::buildSegmentFromCircles(std::vector<glm::vec4>& data, std::vector<glm::vec4>& circ1,
  std::vector<glm::vec4>& circ2){
    if (circ1.size() != circ2.size()) return;

    // Degenerate triangles
    data.push_back(circ1[0]);
    data.push_back(circ1[1]);

    for (int i = 0; i < circ1.size() - 1; i += 2) {
        data.push_back(circ1[i]);
        data.push_back(circ1[i + 1]); // Add the normal
        data.push_back(circ2[i]);
        data.push_back(circ2[i + 1]);
    }

    // Connnect back to start
    data.push_back(circ1[0]);
    data.push_back(circ1[1]);
    data.push_back(circ2[0]);
    data.push_back(circ2[1]);

    // Degenerate triangles
    glm::vec4 last_vert = data[data.size() - 2];
    glm::vec4 last_norm = data[data.size() - 1];
    data.push_back(last_vert);
    data.push_back(last_norm);
}

void ShapeUtil::buildMobiusStrip(std::vector<glm::vec4>& data, int p1, int p2, bool counter_clockwise){
    float delta_v = 2.0f / p1, delta_u = 2.0f * PI / p2, x, y, z, u, v;

    for (int i = 0; i < p2; i++) { // one extra i for connecting back to start
        for (int j = 0; j < p1 + 1; j++) {
            u = i * delta_u;
            v = -1.0f + j * delta_v;
            glm::vec4 pA = mobiusVertex(u, v);
            glm::vec4 pB = mobiusVertex(u + delta_u, v);
            if (counter_clockwise)
                buildMobiusBlock(data, pA, pB, u, v, delta_u, delta_v, p1, j, counter_clockwise);
            else
                buildMobiusBlock(data, pB, pA, u, v, delta_u, delta_v, p1, j, 1 - counter_clockwise);
        }

        // Degenerate triangles
        glm::vec4 last_vert = data[data.size() - 2];
        glm::vec4 last_norm = data[data.size() - 1];
        data.push_back(last_vert);
        data.push_back(last_norm);
    }
} // ShapeUtil::buildMobiusStrip

void ShapeUtil::buildMobiusBlock(std::vector<glm::vec4>& data, glm::vec4 pA, glm::vec4 pB,
  float u, float v, float delta_u, float delta_v, int p1, int j, bool counter_clockwise){
    /* +----H----I----+
     * |    |    |    |
     * |    |    |    |     |                          u space grows rightwards
     * E----A----B----F     | v space grows downwards, ---->
     * |    |    |    |     v
     * |    |    |    |
     * +----C----D----+
     *
     */

    glm::vec4 pC, pD, pE, pF, pH, pI, normA, normB, norm_cur, norm_left, norm_right, norm_up, norm_upleft, norm_upright;
    if (j == 0) {
        // To compute the normal for the inner most vertices, we only need one
        // vertex from the next iteration in v space and one vertex from the next
        // iteration in u space
        pC         = mobiusVertex(u, v + delta_v);
        pD         = mobiusVertex(u + delta_u, v + delta_v);
        pE         = mobiusVertex(u - delta_u, v);
        pF         = mobiusVertex(u + 2 * delta_u, v);
        norm_cur   = normalFromTriangle(pA, pB, pC);
        norm_left  = normalFromTriangle(pE, pA, pC);
        norm_right = normalFromTriangle(pB, pF, pD);
        normA      = interpolate(norm_cur, norm_left, 0.5);
        normB      = interpolate(norm_cur, norm_right, 0.5);

        data.push_back(pA); // Degenerate triangle
        data.push_back(normA);
    } else if (j == p1) {
        // To compute the normal for the outer most vertices (left and right), we
        // need to computed three normals: norm_cur, norm_left, norm_right.
        // Averaging norm_cur with norm_left gives us the normal for the left vertex.
        // Averaging norm_cur with norm_right gives us the normal for the right vertex.
        pH           = data[data.size() - 4];
        pI           = data[data.size() - 2];
        pE           = mobiusVertex(u - delta_u, v);
        pF           = mobiusVertex(u + 2 * delta_u, v);
        norm_up      = normalFromTriangle(pA, pH, pB);
        norm_upleft  = normalFromTriangle(pA, pE, pH);
        norm_upright = normalFromTriangle(pB, pI, pF);
        normA        = interpolate(norm_up, norm_upleft, 0.5);
        normB        = interpolate(norm_up, norm_upright, 0.5);
    } else {
        // To compute the normal for the intermediate vertices, we need the normals
        // from the previous iteration and the next iteration.
        pC           = data[data.size() - 4];
        pD           = data[data.size() - 2];
        pE           = mobiusVertex(u - delta_u, v);
        pF           = mobiusVertex(u + 2 * delta_u, v);
        pH           = mobiusVertex(u, v + delta_v);
        pI           = mobiusVertex(u + delta_u, v + delta_v);
        norm_cur     = normalFromTriangle(pA, pB, pC);
        norm_left    = normalFromTriangle(pE, pA, pC);
        norm_right   = normalFromTriangle(pB, pF, pD);
        norm_up      = normalFromTriangle(pA, pH, pB);
        norm_upleft  = normalFromTriangle(pA, pE, pH);
        norm_upright = normalFromTriangle(pB, pI, pF);

        normA = interpolate(interpolate(norm_cur, norm_left, 0.5), interpolate(norm_up, norm_upleft, 0.5), 0.5);
        normB = interpolate(interpolate(norm_cur, norm_right, 0.5), interpolate(norm_up, norm_upright, 0.5), 0.5);
    }

    data.push_back(pA);
    data.push_back(counter_clockwise ? normA : reverse(normA));
    data.push_back(pB);
    data.push_back(counter_clockwise ? normB : reverse(normB));
} // ShapeUtil::buildMobiusBlock

glm::vec4 ShapeUtil::interpolate(glm::vec4 A, glm::vec4 B, float t){
    float x, y, z, w;

    x = A.x * (1.0 - t) + t * B.x;
    y = A.y * (1.0 - t) + t * B.y;
    z = A.z * (1.0 - t) + t * B.z;
    w = A.w * (1.0 - t) + t * B.w;
    return glm::vec4(x, y, z, w);
}

glm::vec4 ShapeUtil::mobiusVertex(float u, float v){
    float x = (1.0f + v / 2 * glm::cos(u / 2)) * glm::cos(u);
    float y = (1.0f + v / 2 * glm::cos(u / 2)) * glm::sin(u);
    float z = v / 2 * glm::sin(u / 2);

    return glm::vec4(x, y, z, 1);
}

glm::vec4 ShapeUtil::reverse(glm::vec4 v){
    return glm::vec4(-v.x, -v.y, -v.z, v.w);
}

glm::vec4 ShapeUtil::normalFromTriangle(glm::vec4 A, glm::vec4 B, glm::vec4 C){
    return glm::normalize(glm::vec4(glm::normalize(glm::cross(glm::vec3(C - A), glm::vec3(B - A))), 0));
}
