#ifndef SHAPE2D_H
#define SHAPE2D_H

#include <vector>
#include <glm.hpp>

class ShapeUtil
{
public:
    ShapeUtil();

    void buildCircle(std::vector<glm::vec4> &data, int p1, int p2, float radius);

    void buildTriangleStrip(std::vector<glm::vec4> &data, glm::vec4 A, glm::vec4 B, glm::vec4 C, int numSlides);

    void buildQuadStrip(std::vector<glm::vec4> &data, glm::vec4 A, glm::vec4 B, glm::vec4 C, glm::vec4 D, int numQuads);

    void buildSphericalStrip(std::vector<glm::vec4> &data, glm::vec4 A, glm::vec4 B, int numStacks, int numStrips);

    void buildCircleOfVertices(std::vector<glm::vec4> &data, glm::vec4 center, float radius, int numPoints, float phi);

    void buildSegmentFromCircles(std::vector<glm::vec4> &data, std::vector<glm::vec4> &circ1,
      std::vector<glm::vec4> &circ2);

    void buildMobiusStrip(std::vector<glm::vec4> &data, int p1, int p2, bool counter_clockwise);

    void buildMobiusBlock(std::vector<glm::vec4>& data, glm::vec4 pA, glm::vec4 pB,
      float u, float v, float delta_u, float delta_v, int p1, int j, bool counter_clockwise);

    glm::vec4 mobiusVertex(float u, float v);

    glm::vec4 interpolate(glm::vec4 A, glm::vec4 B, float t);

    glm::vec4 reverse(glm::vec4 v);

    glm::vec4 normalFromTriangle(glm::vec4 A, glm::vec4 B, glm::vec4 C);
};

#endif // SHAPE2D_H
