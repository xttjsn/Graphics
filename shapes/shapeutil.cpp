#include "shapeutil.h"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <math.h>

#define PI 3.1415926f

ShapeUtil::ShapeUtil()
{
}

void ShapeUtil::buildCircleUV(std::vector<OpenGLVertex> &data, int p1, int p2, float radius) {
    p1 =  glm::max(p1, 3);
    float x = 0.0f, y = -radius, z = 0.0f, delta = 2.0 * PI / p1, theta = PI / 2.0f;

    glm::vec4 A, B, C;
    A = glm::vec4(x, y, z, 1);
    std::vector<OpenGLVertex> slice;

    for (int i = 0; i < p1; i++) {
        B = glm::vec4(x + radius * glm::cos(theta + i * delta), y, z + radius * glm::sin(theta + i * delta), 1);
        C = glm::vec4(x + radius * glm::cos(theta + (i + 1) * delta), y, z + radius * glm::sin(theta + (i + 1) * delta), 1);
        buildFanStripUV(slice, A, B, C, p2, theta + i * delta, delta);
        for (int j = 0; j < slice.size(); j++) {
            data.push_back(slice[j]);
        }
        slice.clear();
    }
}

void ShapeUtil::buildTriangleStripUV(std::vector<OpenGLVertex> &data, glm::vec4 A, glm::vec4 C, glm::vec4 B, int numStacks, int numSlides, int i) {
    /*       A
     *      / \
     *     /   \
     *    B --- C
     */
    glm::vec4 norm = normalFromTriangle(A, B, C);
    glm::vec2 topUV = rectangleUV((i + 0.5), numStacks, numSlides, numStacks);
    topUV.x = 1.0f - topUV.x;
    data.emplace_back(A, norm, topUV);
    for (int t = 1; t <= numStacks; ++t) {
        glm::vec4 right = interpolate(A, B, t / static_cast<float>(numStacks));
        glm::vec4 left  = interpolate(A, C, t / static_cast<float>(numStacks));
        glm::vec2 rightUV = rectangleUV(i, numStacks - t, numSlides, numStacks);
        glm::vec2 leftUV = rectangleUV((i + 1), numStacks - t, numSlides, numStacks);
        rightUV.x = 1.0f - rightUV.x;
        leftUV.x = 1.0f - leftUV.x;
        data.emplace_back(left, norm, leftUV);
        data.emplace_back(right, norm, rightUV);
    }
    glm::vec2 bottomUV = rectangleUV((i + 0.5), numStacks, numSlides, numStacks);
    bottomUV.x = 1.0f - bottomUV.x;
    data.emplace_back(A, norm, bottomUV);
}

void ShapeUtil::buildFanStripUV(std::vector<OpenGLVertex> &data, glm::vec4 A, glm::vec4 C, glm::vec4 B, int numStacks, float theta, float delta) {
    /*       A
     *      / \
     *     /   \
     *    B --- C
     */
    glm::vec4 norm = normalFromTriangle(A, B, C);
    data.emplace_back(A, norm, circularUV(0, 0));
    for (int t = 1; t <= numStacks; ++t) {
        float ratio = t / static_cast<float>(numStacks);
        glm::vec4 right = interpolate(A, B, ratio);
        glm::vec4 left  = interpolate(A, C, ratio);
        glm::vec2 rightUV = circularUV(theta + delta, ratio);
        glm::vec2 leftUV = circularUV(theta, ratio);
        data.emplace_back(left, norm, leftUV);
        data.emplace_back(right, norm, rightUV);
    }
    data.emplace_back(A, norm, circularUV(0, 0));
}

void ShapeUtil::buildQuadStripUV(std::vector<OpenGLVertex> &data, glm::vec4 Apos, glm::vec4 Bpos, glm::vec4 Cpos, glm::vec4 Dpos,
                      glm::vec2 Auv,  glm::vec2 Buv, glm::vec2 Cuv,  glm::vec2 Duv, int numQuads) {
    /* A ---- C
     * |      |
     * |      |
     * B ---- D
     */
    glm::vec4 norm = normalFromTriangle(Apos, Cpos, Bpos);
    for (int t = 0; t <= numQuads; t++) {
        glm::vec4 upPos   = interpolate(Apos, Cpos, t / static_cast<float>(numQuads));
        glm::vec2 upUV    =  interpolate(Auv, Cuv, t / static_cast<float>(numQuads));
        glm::vec4 downPos = interpolate(Bpos, Dpos, t / static_cast<float>(numQuads));
        glm::vec2 downUV  = interpolate(Buv, Duv, t / static_cast<float>(numQuads));

        data.emplace_back(upPos, norm, upUV);
        data.emplace_back(downPos, norm, downUV);
    }
    data.emplace_back(Dpos, norm, Duv);
    data.emplace_back(Bpos, norm, Buv);
}

void ShapeUtil::buildQuadStripUV(std::vector<OpenGLVertex> &data, glm::vec4 A, glm::vec4 B, glm::vec4 C, glm::vec4 D,
                      int numStacks, int numSlides, int slide) {
    /* A ---- B
     * |      |
     * |      |
     * C ---- D
     */
    glm::vec4 norm = normalFromTriangle(A, B, C);
    for (int t = 0; t <= numStacks; t++) {
        float ratio = t / static_cast<float>(numStacks);
        glm::vec4 leftPos = interpolate(A, C, ratio);
        glm::vec2 leftUV = rectangleUV(slide, numStacks - t, numSlides, numStacks);
        glm::vec4 rightPos = interpolate(B, D, ratio);
        glm::vec2 rightUV = rectangleUV(slide + 1, numStacks - t, numSlides, numStacks);

        data.emplace_back(leftPos, norm, leftUV);
        data.emplace_back(rightPos, norm, rightUV);
    }

    data.emplace_back(D, norm, rectangleUV(slide, 0, numSlides, numStacks));
    data.emplace_back(B, norm, rectangleUV(slide + 1, 0, numSlides, numStacks));
}

void ShapeUtil::buildSphericalStripUV1(std::vector<OpenGLVertex>& data, glm::vec4 A, glm::vec4 B, int numStacks,
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

    data.emplace_back(A, glm::normalize(A), sphericalUV(A - center));

    float phi = 0.0f, theta = 0.0f, delta_phi = 2 * PI / numStrips, delta_theta = PI / numStacks,
          radius = glm::distance(A, B) / 2.0f;
    float x, y, z;
    for (int i = 1; i < numStacks; i++) {
        phi   = 0.0f;
        theta = i * delta_theta;
        x     = radius * glm::sin(theta) * glm::cos(phi);
        y     = radius * glm::sin(theta) * glm::sin(phi);
        z     = radius * glm::cos(theta);
        glm::vec4 leftPos = glm::vec4(x, y, z, 1);
        glm::vec4 leftNorm = glm::normalize(leftPos);
        glm::vec4 leftD = leftPos - center;

        // https://en.wikipedia.org/wiki/UV_mapping
        glm::vec2 leftUV = sphericalUV(leftD);


        phi = delta_phi;
        x   = radius * glm::sin(theta) * glm::cos(phi);
        y   = radius * glm::sin(theta) * glm::sin(phi);
        z   = radius * glm::cos(theta);
        glm::vec4 rightPos = glm::vec4(x, y, z, 1);
        glm::vec4 rightNorm = glm::normalize(rightPos);
        glm::vec4 rightD = rightPos - center;
        glm::vec2 rightUV = sphericalUV(rightD);

        data.emplace_back(leftPos, leftNorm, leftUV);
        data.emplace_back(rightPos, rightNorm, rightUV);
    }

    data.emplace_back(B, glm::normalize(B), sphericalUV(B - center));
} // ShapeUtil::buildSphericalStripUV1

void ShapeUtil::buildSphericalStripUV2(std::vector<OpenGLVertex>& data, glm::vec4 A, glm::vec4 B, int numStacks,
                                    int numStrips, int stripIdx){
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

    data.emplace_back(A, glm::vec4(glm::normalize(glm::vec3(A - center)), 0), rectangleUV(numStrips - (stripIdx + 0.5), numStacks, numStrips, numStacks));

    float phi = stripIdx / static_cast<float>(numStrips) * 2 * PI, theta = 0.0f, delta_phi = 2 * PI / numStrips, delta_theta = PI / numStacks,
          radius = glm::distance(A, B) / 2.0f;
    float x, y, z;
    for (int i = 1; i < numStacks; i++) {
        theta = i * delta_theta;
        x     = radius * glm::sin(theta) * glm::cos(phi);
        z     = radius * glm::sin(theta) * glm::sin(phi);
        y     = radius * glm::cos(theta);
        glm::vec4 leftPos = glm::vec4(x, y, z, 1);
        glm::vec4 leftNorm = glm::vec4(glm::normalize(glm::vec3(leftPos - center)), 0);
        glm::vec2 leftUV = rectangleUV(numStrips - stripIdx, numStacks - i, numStrips, numStacks);

        x   = radius * glm::sin(theta) * glm::cos(phi + delta_phi);
        z   = radius * glm::sin(theta) * glm::sin(phi + delta_phi);
        y   = radius * glm::cos(theta);
        glm::vec4 rightPos = glm::vec4(x, y, z, 1);
        glm::vec4 rightNorm = glm::vec4(glm::normalize(glm::vec3(rightPos - center)), 0);
        glm::vec2 rightUV = rectangleUV(numStrips - (stripIdx + 1), numStacks - i, numStrips, numStacks);

        data.emplace_back(rightPos, rightNorm, rightUV);
        data.emplace_back(leftPos, leftNorm, leftUV);
    }

    data.emplace_back(B, glm::vec4(glm::normalize(glm::vec3(B - center)), 0), rectangleUV(numStrips - (stripIdx + 0.5), 0, numStrips, numStacks));
}

void ShapeUtil::buildCircleOfVerticesUV(std::vector<OpenGLVertex> &data, glm::vec4 center, float radius, int numPoints, float phi, int numSegs, int seg) {
    float delta = 2 * PI / numPoints, theta = 0.0f, x, y, z;

    glm::vec4 norm, pos;
    glm::vec2 UV;

    for (int i = 0; i < numPoints; i++) {
        theta = i * delta;
        x     = radius * glm::sin(theta) * glm::cos(phi);
        y     = radius * glm::sin(theta) * glm::sin(phi);
        z     = radius * glm::cos(theta);
        pos = glm::translate(glm::vec3(center)) * glm::vec4(x, y, z, 1);
        norm = glm::normalize(pos - center);
        UV = rectangleUV(seg, i, numSegs, numPoints);
        data.emplace_back(pos, norm, UV);
    }
}

void ShapeUtil::buildSegmentFromCirclesUV(std::vector<OpenGLVertex>& data, std::vector<OpenGLVertex>& circ1,
                                          std::vector<OpenGLVertex>& circ2) {
    if (circ1.size() != circ2.size()) return;

    // Degenerate triangles
    data.push_back(circ1[0]);

    int sz = circ1.size();
    for (int i = 0; i < sz; i++) {
        data.push_back(circ1[i]);
        data.push_back(circ2[i]);
    }

    // Connect back to start
    data.push_back(circ1[0]);
    data.push_back(circ2[0]);

    // Degenerate triangles
    data.push_back(data[data.size() - 1]);
}

void ShapeUtil::buildMobiusStrip(std::vector<glm::vec4>& data, int p1, int p2, bool clockwise){
    float delta_v = 2.0f / p1, delta_u = 2.0f * PI / p2, u, v;

    for (int i = 0; i < p2; i++) { // one extra i for connecting back to start
        for (int j = 0; j < p1 + 1; j++) {
            u = i * delta_u;
            v = -1.0f + j * delta_v;
            glm::vec4 pA = mobiusVertex(u, v);
            glm::vec4 pB = mobiusVertex(u + delta_u, v);
            buildMobiusBlock(data, pA, pB, u, v, delta_u, delta_v, p1, p2, i, j, clockwise);
        }

        // Degenerate triangles
        glm::vec4 last_vert = data[data.size() - 2];
        glm::vec4 last_norm = data[data.size() - 1];
        data.push_back(last_vert);
        data.push_back(last_norm);
    }
} // ShapeUtil::buildMobiusStrip

void ShapeUtil::buildMobiusBlock(std::vector<glm::vec4>& data, glm::vec4 pA, glm::vec4 pB,
                                 float u, float v, float delta_u, float delta_v, int p1, int p2, int i, int j, bool clockwise){
    /* Clock wise
     * +----G----H----J
     * |   /|   /|   /|
     * | / c|b/a |j/i |     |                          u space grows rightwards
     * E----A----B----F     | v space grows downwards, ---->
     * |  d/|f  /|h  /|     v
     * | / e| / g| /  |
     * I----C----D----+
     *
     * Counter clock wise
     * J----G----H----+
     * | \  | \  | \  |
     * | c\b|a \j|i \ |     |                          u space grows rightwards
     * E----A----B----F     | v space grows downwards, ---->
     * | \d |e\f |g\h |     v
     * |  \ |  \ |  \ |
     * +----C----D----I
     */

    glm::vec4 pC, pD, pE, pF, pG, pH, pI, pJ, normA, normB, norm_a, norm_b, norm_c, norm_d, norm_e, norm_f, norm_g,
              norm_h, norm_i, norm_j;
    std::vector<glm::vec4> vs;

    pC = mobiusVertex(u, v + delta_v);
    pD = mobiusVertex(u + delta_u, v + delta_v);
    pE = mobiusVertex(u - delta_u, v);
    pF = mobiusVertex(u + 2 * delta_u, v);
    pG = mobiusVertex(u, v - delta_v);
    pH = mobiusVertex(u + delta_u, v - delta_v);
    if (clockwise) {
        pI     = mobiusVertex(u - delta_u, v + delta_v);
        pJ     = mobiusVertex(u + 2 * delta_u, v - delta_v);
        norm_a = normalFromTriangle(pB, pA, pH);
        norm_b = normalFromTriangle(pG, pH, pA);
        norm_c = normalFromTriangle(pA, pE, pG);
        norm_d = normalFromTriangle(pE, pA, pI);
        norm_e = normalFromTriangle(pC, pI, pA);
        norm_f = normalFromTriangle(pA, pB, pC);
        norm_g = normalFromTriangle(pD, pC, pB);
        norm_h = normalFromTriangle(pB, pF, pD);
        norm_i = normalFromTriangle(pF, pB, pJ);
        norm_j = normalFromTriangle(pH, pJ, pH);
    } else {
        pI     = mobiusVertex(u + 2 * delta_u, v + delta_v);
        pJ     = mobiusVertex(u - delta_u, v - delta_v);
        norm_a = normalFromTriangle(pA, pB, pG);
        norm_b = normalFromTriangle(pA, pG, pJ);
        norm_c = normalFromTriangle(pA, pJ, pE);
        norm_d = normalFromTriangle(pA, pE, pC);
        norm_e = normalFromTriangle(pA, pC, pD);
        norm_f = normalFromTriangle(pA, pD, pB);
        norm_g = normalFromTriangle(pB, pD, pI);
        norm_h = normalFromTriangle(pB, pI, pF);
        norm_i = normalFromTriangle(pB, pF, pH);
        norm_j = normalFromTriangle(pB, pH, pG);
    }

    if (j == 0) {
        // To compute the normal for the inner most vertices, we only need one
        // vertex from the next iteration in v space and one vertex from the next
        // iteration in u space
        vs    = { norm_d, norm_e, norm_f };
        normA = average(vs);
        vs    = { norm_f, norm_g, norm_h };
        normB = average(vs);

        // Degenerate triangle
        if (clockwise) {
            data.push_back(pA);
            data.push_back(normA);
        } else {
            data.push_back(pB);
            data.push_back(normB);
        }
    } else if (j == p1) {
        // To compute the normal for the outer most vertices (left and right), we
        // need to computed three normals: norm_cur, norm_left, norm_right.
        // Averaging norm_cur with norm_left gives us the normal for the left vertex.
        // Averaging norm_cur with norm_right gives us the normal for the right vertex.
        vs    = { norm_a, norm_b, norm_c };
        normA = average(vs);
        vs    = { norm_a, norm_i, norm_j };
        normB = average(vs);
        // normA = normB = glm::vec4(0, 0, 0, 0);
    } else {
        // To compute the normal for the intermediate vertices, we need the normals
        // from the previous iteration and the next iteration.
        vs    = { norm_a, norm_b, norm_c, norm_d, norm_e, norm_f };
        normA = average(vs);
        vs    = { norm_i, norm_j, norm_a, norm_f, norm_g, norm_h };
        normB = average(vs);
        // normA = normB = glm::vec4(0, 0, 0, 0);
    }

    if (clockwise) {
        data.push_back(pA);
        data.push_back(normA);
        data.push_back(pB);
        data.push_back(normB);
    } else {
        data.push_back(pB);
        data.push_back(normB);
        data.push_back(pA);
        data.push_back(normA);
    }
} // ShapeUtil::buildMobiusBlock

glm::vec4 ShapeUtil::interpolate(glm::vec4 A, glm::vec4 B, float t){
    float x, y, z, w;

    x = A.x * (1.0 - t) + t * B.x;
    y = A.y * (1.0 - t) + t * B.y;
    z = A.z * (1.0 - t) + t * B.z;
    w = A.w * (1.0 - t) + t * B.w;
    return glm::vec4(x, y, z, w);
}

glm::vec2 ShapeUtil::interpolate(glm::vec2 A, glm::vec2 B, float t) {
    float x, y;
    x = A.x * (1.0 - t) + t * B.x;
    y = A.y * (1.0 - t) + t * B.y;
    return glm::vec2(x, y);
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

glm::vec4 ShapeUtil::average(std::vector<glm::vec4>& vertices){
    float x = 0.0f, y = 0.0f, z = 0.0f;

    for (glm::vec4& v : vertices) {
        x += v.x;
        y += v.y;
        z += v.z;
    }
    return glm::normalize(glm::vec4(x / vertices.size(), y / vertices.size(), z / vertices.size(), vertices[0].w));
}

glm::vec4 ShapeUtil::normalFromTriangle(glm::vec4 A, glm::vec4 B, glm::vec4 C){
    return glm::normalize(glm::vec4(glm::normalize(glm::cross(glm::vec3(C - A), glm::vec3(B - A))), 0));
}

glm::vec2 ShapeUtil::sphericalUV(glm::vec4 dist) {
    glm::vec3 d = glm::normalize(glm::vec3(dist));
    return glm::vec2(glm::clamp((std::atan2(d.z, d.x) / PI + 1.0f) * 0.5f, 0.0f, 1.0f),
                     0.5 - std::asin(d.y) / PI);
}

glm::vec2 ShapeUtil::rectangleUV(float i, float j, float n, float m) {
    return glm::vec2(i / n, j / m);
}

glm::vec2 ShapeUtil::circularUV(float theta, float ratio) {
    return glm::vec2(0.5 + 0.5 * ratio * glm::cos(theta), 0.5 + 0.5 * ratio * glm::sin(theta));
}
