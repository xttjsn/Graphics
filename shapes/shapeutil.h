#ifndef SHAPE2D_H
#define SHAPE2D_H

#include <vector>
#include <glm.hpp>

class ShapeUtil
{
public:
    ShapeUtil();

    void buildCircle(std::vector<glm::vec4>& data, int p1, int p2, float radius);

    void buildTriangleStrip(std::vector<glm::vec4>& data, glm::vec4 A, glm::vec4 B, glm::vec4 C, int numSlides);

    void buildQuadStrip(std::vector<glm::vec4>& data, glm::vec4 A, glm::vec4 B, glm::vec4 C, glm::vec4 D, int numQuads);

    void buildSphericalStrip(std::vector<glm::vec4>& data, glm::vec4 A, glm::vec4 B, int numStacks, int numStrips);

    void buildCircleOfVertices(std::vector<glm::vec4>& data, glm::vec4 center, float radius, int numPoints, float phi);

    void buildSegmentFromCircles(std::vector<glm::vec4>& data, std::vector<glm::vec4>& circ1,
      std::vector<glm::vec4>& circ2);

    glm::vec4 interpolate(glm::vec4 A, glm::vec4 B, float t);
};

#endif // SHAPE2D_H
