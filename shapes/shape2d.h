#ifndef SHAPE2D_H
#define SHAPE2D_H

#include <vector>
#include <glm.hpp>

class Shape2D
{
public:
    Shape2D();
    void buildCircle(std::vector<glm::vec4>& data, int p1, int p2, float radius);
    void buildTriangleStrip(std::vector<glm::vec4>& data, glm::vec4 A, glm::vec4 B, glm::vec4 C, int numSlides);
    void buildQuadStrip(std::vector<glm::vec4>& data, glm::vec4 A, glm::vec4 B, glm::vec4 C, glm::vec4 D, int numQuads);
    glm::vec4 interpolate(glm::vec4 A, glm::vec4 B, float t);
};

#endif // SHAPE2D_H
