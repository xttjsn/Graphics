#ifndef IMPLICITSHAPE_H
#define IMPLICITSHAPE_H

#include <glm.hpp>
#include <glm/gtx/epsilon.hpp>
#include <vector>
#include <math.h>
#include "lib/CS123SceneData.h"

#define PI 3.14159265f
#define EPSILON 1e-4   // The epsilon used in floating point comparison
#define fequal(a, b) (glm::gtx::epsilon::equalEpsilon((a), (b), EPSILON))

struct Ray
{
    glm::vec4 start;
    glm::vec4 delta;
    Ray(const glm::vec4& Astart, const glm::vec4& Adelta)
        : start(Astart), delta(Adelta) {}
};

struct Intersect
{
    bool miss;
    glm::vec4 pos;
    Intersect(bool Amiss, const glm::vec4& Apos)
        : miss(Amiss), pos(Apos) {}
};

struct BoundingBox
{
    float xMin;
    float xMax;
    float yMin;
    float yMax;
    float zMin;
    float zMax;
    BoundingBox(float AxMin, float AxMax, float AyMin, float AyMax, float AzMin, float AzMax)
        : xMin(AxMin), xMax(AxMax), yMin(AyMin), yMax(AyMax), zMin(AzMin), zMax(AzMax) {}
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
    virtual glm::vec4 diffuseAtIntersect(Intersect& intersect, CS123SceneLightData& light, CS123SceneGlobalData& global) = 0;
    virtual float surfaceArea() = 0;
    virtual BoundingBox boundingBox() = 0;
    void setTransform(const glm::mat4x4& transform);
    void setMaterial(const CS123SceneMaterial& material);

protected:
    glm::mat4x4 m_transform;
    glm::mat4x4 m_transform_inv;
    CS123SceneMaterial m_material;
};

#endif // IMPLICITSHAPE_H
