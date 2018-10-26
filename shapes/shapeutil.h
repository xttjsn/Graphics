#ifndef SHAPE2D_H
#define SHAPE2D_H

#include <vector>
#include <glm.hpp>
#include "openglshape.h"

class ShapeUtil
{
public:
    ShapeUtil();

    void buildCircleUV(std::vector<OpenGLVertex> &data, int p1, int p2, float radius);

    void buildFanStripUV(std::vector<OpenGLVertex> &data, glm::vec4 A, glm::vec4 C, glm::vec4 B, int numStacks, float theta, float delta);

    void buildTriangleStripUV(std::vector<OpenGLVertex> &data, glm::vec4 A, glm::vec4 C, glm::vec4 B, int numStacks, int numSlides, int i);

    void buildQuadStripUV(std::vector<OpenGLVertex> &data, glm::vec4 Apos, glm::vec4 Bpos, glm::vec4 Cpos, glm::vec4 Dpos,
                          glm::vec2 Auv,  glm::vec2 Byuv, glm::vec2 Cuv,  glm::vec2 Duv, int numQuads);

    void buildQuadStripUV(std::vector<OpenGLVertex> &data, glm::vec4 A, glm::vec4 B, glm::vec4 C, glm::vec4 D,
                          int numStacks, int numSlides, int slide);

    void buildSphericalStripUV1(std::vector<OpenGLVertex>& data, glm::vec4 A, glm::vec4 B, int numStacks, int numStrips);

    void buildSphericalStripUV2(std::vector<OpenGLVertex>& data, glm::vec4 A, glm::vec4 B, int numStacks, int numStrips, int stripIdx);

    void buildCircleOfVerticesUV(std::vector<OpenGLVertex> &data, glm::vec4 center, float radius, int numPoints, float phi, int numSegs, int seg);

    void buildSegmentFromCirclesUV(std::vector<OpenGLVertex> &data, std::vector<OpenGLVertex> &circ1,
                                 std::vector<OpenGLVertex> &circ2);

    void buildMobiusStrip(std::vector<glm::vec4> &data, int p1, int p2, bool clockwise);

    void buildMobiusBlock(std::vector<glm::vec4>& data, glm::vec4 pA, glm::vec4 pB,
                          float u, float v, float delta_u, float delta_v, int p1, int p2, int i, int j, bool clockwise);

    glm::vec4 mobiusVertex(float u, float v);

    glm::vec4 interpolate(glm::vec4 A, glm::vec4 B, float t);

    glm::vec2 interpolate(glm::vec2 A, glm::vec2 B, float t);

    glm::vec4 reverse(glm::vec4 v);

    glm::vec4 average(std::vector<glm::vec4>& vertices);

    glm::vec4 normalFromTriangle(glm::vec4 A, glm::vec4 B, glm::vec4 C);

    glm::vec2 sphericalUV(glm::vec4 d);

    glm::vec2 rectangleUV(float i, float j, float n, float m);

    glm::vec2 circularUV(float theta, float ratio);
};

#endif // SHAPE2D_H
