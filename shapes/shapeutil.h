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
    glm::vec4 interpolate(glm::vec4 A, glm::vec4 B, float t);
};

#endif // SHAPE2D_H
