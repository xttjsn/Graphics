#include "shape2d.h"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#define PI 3.1415926f

Shape2D::Shape2D()
{
}

void Shape2D::buildCircle(std::vector<glm::vec4> &data, int p1, int p2, float radius) {

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

void Shape2D::buildTriangleStrip(std::vector<glm::vec4>& data, glm::vec4 A, glm::vec4 B, glm::vec4 C, int numSlides) {
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
        glm::vec4 left = interpolate(A, C, t / static_cast<float>(numSlides));
        data.push_back(right);
        data.push_back(norm4);
        data.push_back(left);
        data.push_back(norm4);
    }
    data.push_back(A);
    data.push_back(norm4);
}

void Shape2D::buildQuadStrip(std::vector<glm::vec4>& data, glm::vec4 A, glm::vec4 B, glm::vec4 C, glm::vec4 D, int numQuads) {
    /* A ---- C
     * |      |
     * |      |
     * B ---- D
     */
    glm::vec3 norm3 = glm::normalize(glm::cross(glm::vec3(A - C), glm::vec3(B - A)));
    glm::vec4 norm4 = glm::vec4(norm3, 0);
    for (int t = 0; t <= numQuads; t++) {
        glm::vec4 up = interpolate(A, C, t / static_cast<float>(numQuads));
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

glm::vec4 Shape2D::interpolate(glm::vec4 A, glm::vec4 B, float t) {
    float *dataA = glm::value_ptr(A), *dataB = glm::value_ptr(B), x, y, z;
    x = *(dataA + 0) * (1.0 - t) + t * (*(dataB + 0));
    y = *(dataA + 1) * (1.0 - t) + t * (*(dataB + 1));
    z = *(dataA + 2) * (1.0 - t) + t * (*(dataB + 2));
    return glm::vec4(x, y, z, 1);
}
