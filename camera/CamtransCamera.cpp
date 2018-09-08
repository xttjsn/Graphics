/**
 * @file   CamtransCamera.cpp
 *
 * This is the perspective camera class you will need to fill in for the Camtrans lab.  See the
 * lab handout for more details.
 */

#include "CamtransCamera.h"
#include <Settings.h>

CamtransCamera::CamtransCamera()
{
    // @TODO Task 3: [CAMTRANS] Set up the default camera settings to match the demo...
}

void CamtransCamera::setAspectRatio(float a)
{
    m_ar = a;
}

glm::mat4x4 CamtransCamera::getProjectionMatrix() const {
    return getPerspectiveMatrix() * getScaleMatrix();
}

glm::mat4x4 CamtransCamera::getViewMatrix() const {
    return glm::transpose(glm::mat4x4(m_u.x, m_u.y, m_u.z, 0,
                       m_v.x, m_v.y, m_v.z, 0,
                       m_w.x, m_w.y, m_w.z, 0,
                       0, 0, 0, 1)) *

           glm::transpose(glm::mat4x4(1, 0, 0, -m_eye.x,
                       0, 1, 0, -m_eye.y,
                       0, 0, 1, -m_eye.z,
                       0, 0, 0, 1));
}

glm::mat4x4 CamtransCamera::getScaleMatrix() const {
    float fr = 1.f / m_far;
    float ftr = fr / tan(glm::radians(m_ha / 2.f));

    return glm::mat4x4(ftr / m_ar, 0, 0, 0,
                       0, ftr, 0, 0,
                       0, 0, fr, 0,
                       0, 0, 0, 1);
}

glm::mat4x4 CamtransCamera::getPerspectiveMatrix() const {
    float c = -m_near / m_far;

    return glm::transpose(glm::mat4x4(1, 0, 0, 0,
                       0, 1, 0, 0,
                       0, 0, -1/(c+1), c/(c+1),
                       0, 0, -1, 0));
}

glm::vec4 CamtransCamera::getPosition() const {
    return m_eye;
}

glm::vec4 CamtransCamera::getU() const {
    return m_u;
}

glm::vec4 CamtransCamera::getV() const {
    return m_v;
}

glm::vec4 CamtransCamera::getW() const {
    return m_w;
}

glm::vec4 CamtransCamera::getLook() const {
    return -m_w;
}

float CamtransCamera::getAspectRatio() const {
    return m_ar;
}

float CamtransCamera::getHeightAngle() const {
    return m_ha;
}

void CamtransCamera::orientLook(const glm::vec4 &eye, const glm::vec4 &look, const glm::vec4 &up) {

      m_eye = eye;
      m_w = -glm::normalize(look);
      m_v = glm::normalize(up - m_w * glm::dot(m_w, up));
      m_u = glm::vec4(glm::cross(m_v.xyz(), m_w.xyz()), 0);
}

void CamtransCamera::setHeightAngle(float h) {
      m_ha = h;
}

void CamtransCamera::translate(const glm::vec4 &v) {
    m_eye += v;
}

void CamtransCamera::rotateU(float degrees) {
    float rads = glm::radians(degrees);
    float s = glm::sin(rads);
    float c = glm::cos(rads);

    glm::vec4 v = m_v * c + m_w * s;
    glm::vec4 w = -m_v * s + m_w * c;

    m_v = v;
    m_w = w;
}

void CamtransCamera::rotateV(float degrees) {
    float rads = glm::radians(degrees);
    float s = glm::sin(rads);
    float c = glm::cos(rads);

    glm::vec4 u = m_u * c - m_w * s;
    glm::vec4 w = m_u * s + m_w * c;

    m_u = u;
    m_w = w;
}

void CamtransCamera::rotateW(float degrees) {
    float rads = glm::radians(degrees);
    float s = glm::sin(rads);
    float c = glm::cos(rads);

    glm::vec4 v = m_v * c - m_u * s;
    glm::vec4 u = m_v * s + m_u * c;

    m_u = u;
    m_v = v;
}

void CamtransCamera::setClip(float nearPlane, float farPlane) {
    m_near = nearPlane;
    m_far = farPlane;
}

// @TODO Task 1: Define the helper methods for updating the matrices here...
// @TODO Task 2: Fill in the helper methods you created for updating the matrices...
