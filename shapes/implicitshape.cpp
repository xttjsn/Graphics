#include "implicitshape.h"

BoundingBox::BoundingBox(const BoundingBox& that)
    : xMin(that.xMin), xMax(that.xMax),
      yMin(that.yMin), yMax(that.yMax),
      zMin(that.zMin), zMax(that.zMax), transprim(that.transprim) {}

KDTreeNode::KDTreeNode() {
}

KDTreeNode::~KDTreeNode() {
    if (left)
        delete left;
    if (right)
        delete right;
}

ImplicitShape::ImplicitShape()
{}


void ImplicitShape::setTransform(const glm::mat4x4 &transform) {
    m_transform = transform;
    m_transform_inv = glm::inverse(transform);
}

void ImplicitShape::setMaterial(const CS123SceneMaterial &material) {
    m_material = material;
}

glm::vec4 ImplicitShape::diffuseAtIntersect(Intersect& intersect, CS123SceneLightData& light, CS123SceneGlobalData& global) {
    // Assume intersect is valid. We don't perform any test for its validity.
    if (intersect.miss) return glm::vec4(0);

    glm::vec4 lightColor = light.color;
    glm::vec4 objectColor = m_material.cDiffuse;
    glm::vec4 lightPos = light.pos;
    glm::vec4 interPos = intersect.pos;
    glm::vec4 lightVec = glm::normalize(lightPos - interPos);
    glm::vec4 norm = normal(intersect);

    float cos = glm::dot(lightVec, norm);
    float r = lightColor.x * global.kd * objectColor.x * cos;
    float g = lightColor.y * global.kd * objectColor.y * cos;
    float b = lightColor.z * global.kd * objectColor.z * cos;

    return glm::vec4(r, g, b, 1);
}
