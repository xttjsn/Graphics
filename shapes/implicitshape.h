#ifndef IMPLICITSHAPE_H
#define IMPLICITSHAPE_H

#include <glm.hpp>
#include <vector>
#include "lib/CS123SceneData.h"

struct Ray
{
    glm::vec4 start;
    glm::vec4 delta;
};

struct Intersect
{
    bool miss;
    glm::vec4 pos;
};

struct BoundingBox
{
    float xMin;
    float xMax;
    float yMin;
    float yMax;
    float zMin;
    float zMax;
};

class KDTreeNode
{
public:
    KDTreeNode();

    void split();

    bool leaf;
    KDTreeNode left;
    KDTreeNode right;

    BoundingBox bbox;
    std::vector<CS123ScenePrimitive> primitives;
};

class ImplicitShape
{
public:
    ImplicitShape();

    virtual Intersect intersect(const Ray& ray) = 0;
    virtual glm::vec4 normal(Intersect& intersect) = 0;
    virtual glm::vec4 diffuseAtIntersect(Intersect& intersect, CS123SceneLightData& light) = 0;
    virtual float surfaceArea() = 0;
    virtual BoundingBox boundingBox() = 0;

protected:
    glm::mat4x4 m_transform;
    CS123SceneMaterial m_material;
};

#endif // IMPLICITSHAPE_H
