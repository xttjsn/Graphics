#include "implicitshape.h"

ImplicitShape::ImplicitShape()
{
}

ImplicitShape::setTransform(const glm::mat4x4 &transform) {
    m_transform = transform;
    m_transform_inv = glm::inverse(transform);
}
