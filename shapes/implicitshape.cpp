#include "implicitshape.h"
#include "glm/gtx/transform.hpp"

BoundingBox::BoundingBox()
    : xMin(FLT_MAX), xMax(FLT_MIN), yMin(FLT_MAX), yMax(FLT_MIN), zMin(FLT_MAX), zMax(FLT_MIN), transprim(nullptr) {
    transform = transformInv = glm::mat4x4();
}

BoundingBox::BoundingBox(float AxMin, float AxMax, float AyMin, float AyMax, float AzMin, float AzMax)
        : xMin(AxMin), xMax(AxMax), yMin(AyMin), yMax(AyMax), zMin(AzMin), zMax(AzMax), transprim(nullptr) {
    float xc = (xMax + xMin) / 2, yc = (yMax + yMin) / 2, zc = (zMax + zMin) / 2;
    float xs = xMax - xMin, ys = yMax - yMin, zs = zMax - zMin;
    transform = glm::translate(glm::vec3(xc, yc, zc)) * glm::scale(glm::vec3(xs, ys, zs));
    transformInv = glm::inverse(transform);
}

BoundingBox::BoundingBox(const BoundingBox& that)
    : xMin(that.xMin), xMax(that.xMax),
      yMin(that.yMin), yMax(that.yMax),
      zMin(that.zMin), zMax(that.zMax), transprim(that.transprim), transform(that.transform), transformInv(that.transformInv) {}

KDTreeNode::KDTreeNode()
    : left(nullptr), right(nullptr) {}

KDTreeNode::~KDTreeNode() {
    if (left)
        delete left;
    if (right)
        delete right;
}

ImplicitShape::ImplicitShape()
{}


void ImplicitShape::setTransform(const glm::mat4x4& transform, const glm::mat4x4& transformInv) {
    m_transform = transform;
    m_transform_inv = transformInv;
}

void ImplicitShape::setMaterial(const CS123SceneMaterial &material) {
    m_material = material;
}
