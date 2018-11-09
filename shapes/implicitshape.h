#ifndef IMPLICITSHAPE_H
#define IMPLICITSHAPE_H

#include <glm.hpp>
#include <glm/gtx/epsilon.hpp>
#include <vector>
#include <math.h>
#include "lib/CS123SceneData.h"

#define EPSILON 1e-9f   // The epsilon used in floating point comparison
#define fequal(a, b) (glm::epsilonEqual((a), (b), EPSILON))

struct Ray
{
    glm::vec4 start;
    glm::vec4 delta;
    Ray() : start(glm::vec4(0)), delta(glm::vec4(0)) {}
    Ray(const glm::vec4& Astart, const glm::vec4& Adelta)
        : start(Astart), delta(Adelta) {}
};

struct Intersect
{
    bool miss;
    glm::vec4 pos;
    float t;
    CS123TransformPrimitive* transprim;
    Intersect() : miss(true), pos(glm::vec4(0)), t(FLT_MAX) {}
    Intersect(const bool Amiss, const glm::vec4& Apos, float At)
        : miss(Amiss), pos(Apos), t(At), transprim(nullptr) {}
};

struct itsctComp
{
    inline bool operator() (const Intersect& left, const Intersect& right) {
        return left.t < right.t;
    }
};

struct BoundingBox
{
    int id = 0;
    float xMin;
    float xMax;
    float yMin;
    float yMax;
    float zMin;
    float zMax;
    CS123TransformPrimitive* transprim;
    BoundingBox()
        : xMin(FLT_MAX), xMax(FLT_MIN), yMin(FLT_MAX), yMax(FLT_MIN), zMin(FLT_MAX), zMax(FLT_MIN), transprim(nullptr) {}
    BoundingBox(float AxMin, float AxMax, float AyMin, float AyMax, float AzMin, float AzMax)
        : xMin(AxMin), xMax(AxMax), yMin(AyMin), yMax(AyMax), zMin(AzMin), zMax(AzMax), transprim(nullptr) {}
    BoundingBox(const BoundingBox& that);
};

struct KDTreePrimitive
{
    float surface;
    BoundingBox bbox;
    CS123TransformPrimitive* transprim; // KDTreePrimitive do not own this pointer
};

class KDTreeNode
{
public:
    KDTreeNode();
    ~KDTreeNode();
    KDTreeNode* left;   // Parent owns left
    KDTreeNode* right;  // Parent owns right
    BoundingBox bbox;
    float surface;
    std::vector<KDTreePrimitive> primitives;
};

enum Axis {
    AXIS_X,
    AXIS_Y,
    AXIS_Z
};

class ImplicitShape
{
public:
    ImplicitShape();
    virtual ~ImplicitShape() {}

    virtual Intersect intersect(const Ray& ray) = 0;
    virtual glm::vec4 normal(Intersect& intersect) = 0;
    virtual glm::vec4 diffuseAtIntersect(Intersect& intersect, CS123SceneLightData& light, CS123SceneGlobalData& global);
    virtual float surfaceArea() = 0;
    virtual BoundingBox boundingBox() = 0;
    void setTransform(const glm::mat4x4& transform);
    void setMaterial(const CS123SceneMaterial& material);

protected:
    glm::mat4x4 m_transform;
    glm::mat4x4 m_transform_inv;
    CS123SceneMaterial m_material;
};

struct less_than_x_key
{
    inline bool operator() (const KDTreePrimitive& prim1, const KDTreePrimitive& prim2) {
        return (prim1.bbox.xMin < prim2.bbox.xMin);
    }
};

struct less_than_y_key
{
    inline bool operator() (const KDTreePrimitive& prim1, const KDTreePrimitive& prim2) {
        return (prim1.bbox.yMin < prim2.bbox.yMin);
    }
};

struct less_than_z_key
{
    inline bool operator() (const KDTreePrimitive& prim1, const KDTreePrimitive& prim2) {
        return (prim1.bbox.zMin < prim2.bbox.zMin);
    }
};

#endif // IMPLICITSHAPE_H
