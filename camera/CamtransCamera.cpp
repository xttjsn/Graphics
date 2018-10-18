/**
 * @file   CamtransCamera.cpp
 *
 * This is the perspective camera class you will need to fill in for the Camtrans lab.  See the
 * lab handout for more details.
 */

#include "CamtransCamera.h"
#include <Settings.h>

CamtransCamera::CamtransCamera(){
    // @TODO Task 3: [CAMTRANS] Set up the default camera settings to match the demo...
    m_near = 1.0f;
    m_far = 30.0f;
    m_thetaH = 60.0f;
    m_aspectRatio = 1.0f;
    //              Eye                     look                        Up
    orientLook(glm::vec4(2, 2, 2, 1), glm::vec4(-2, -2, -2, 1), glm::vec4(0, 1, 0, 1));
}

glm::mat4x4 CamtransCamera::getProjectionMatrix() const {
    // TODO Task 3: Camtrans
    return m_perspectiveTransformation * m_scaleMatrix;
}

glm::mat4x4 CamtransCamera::getViewMatrix() const {
    // TODO Task 3: Camtrans
    return m_rotationMatrix * m_translationMatrix;
}

glm::mat4x4 CamtransCamera::getScaleMatrix() const {
    // TODO Task 1: Camtrans
    return m_scaleMatrix;
}

glm::mat4x4 CamtransCamera::getPerspectiveMatrix() const {
    // TODO Task 1: Camtrans
    return m_perspectiveTransformation;
}

glm::vec4 CamtransCamera::getPosition() const {
    // TODO Task 1: Camtrans
    return m_eye;
}

glm::vec4 CamtransCamera::getU() const {
    // TODO Task 1: Camtrans
    return m_u;
}

glm::vec4 CamtransCamera::getV() const {
    // TODO Task 1: Camtrans
    return m_v;
}

glm::vec4 CamtransCamera::getW() const {
    // TODO Task 1: Camtrans
    return m_w;
}

glm::vec4 CamtransCamera::getLook() const {
    // TODO Task 1: Camtrans
    return -m_w;
}

float CamtransCamera::getAspectRatio() const {
    // TODO Task 1: Camtrans
    return m_aspectRatio;
}

float CamtransCamera::getHeightAngle() const {
    // TODO Task 1: Camtrans
    return m_thetaH;
}

void CamtransCamera::orientLook(const glm::vec4 &eye, const glm::vec4 &look, const glm::vec4 &up){
    // TODO Task 3: Camtrans
    m_eye = eye;
    m_w = -glm::normalize(look);
    m_up = up;
    m_v = glm::normalize(m_up - glm::dot(m_up, m_w) * m_w);
    m_u = glm::vec4(glm::cross(glm::vec3(m_v), glm::vec3(m_w)), 1);
    updateViewMatrix();
    updateProjectionMatrix();
}

void CamtransCamera::setHeightAngle(float h){
    // TODO Task 3: Camtrans
    m_thetaH = h;
    updateProjectionMatrix();
}

void CamtransCamera::setAspectRatio(float a){
    // TODO Task 3: Camtrans
    m_aspectRatio = a;
    updateProjectionMatrix();
}

void CamtransCamera::translate(const glm::vec4 &v){
    // TODO: Camtrans
    m_eye += v;
    updateViewMatrix();
}

void CamtransCamera::rotateU(float degrees){
    // TODO Task 3: Camtrans
    float rad = glm::radians(degrees);
    glm::vec4 v0 = m_v, w0 = m_w;
    m_v = v0 * glm::cos(rad) + w0 * glm::sin(rad);
    m_w = w0 * glm::cos(rad) - v0 * glm::sin(rad);
    updateViewMatrix();
}

void CamtransCamera::rotateV(float degrees){
    // TODO Task 3: Camtrans
    float rad = glm::radians(degrees);
    glm::vec4 u0 = m_u, w0 = m_w;
    m_u = u0 * glm::cos(rad) + w0 * glm::sin(rad);
    m_w = w0 * glm::cos(rad) - u0 * glm::sin(rad);
    updateViewMatrix();
}

void CamtransCamera::rotateW(float degrees){
    // TODO Task 3: Camtrans
    float rad = glm::radians(degrees);
    glm::vec4 u0 = m_u, v0 = m_v;
    m_u = u0 * glm::cos(rad) + v0 * glm::sin(rad);
    m_v = v0 * glm::cos(rad) - u0 * glm::sin(rad);
    updateViewMatrix();
}

void CamtransCamera::setClip(float nearPlane, float farPlane){
    // TODO Task 3: Camtrans
    m_near = nearPlane;
    m_far = farPlane;
    updateProjectionMatrix();
}

// @TODO Task 1: Define the helper methods for updating the matrices here...
void CamtransCamera::updateProjectionMatrix(){
    updateScaleMatrix();
    updatePerspectiveMatrix();
}

void CamtransCamera::updatePerspectiveMatrix(){
    float c = - m_near / m_far;
    m_perspectiveTransformation = glm::transpose(glm::mat4(
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1 / (1 + c), (-c) / (1 + c),
        0, 0, -1, 0
    ));
}

void CamtransCamera::updateScaleMatrix(){
    float tan_h = glm::tan(glm::radians(m_thetaH) / 2.0);
    m_scaleMatrix = glm::transpose(glm::mat4(
        1.0 / (m_far * m_aspectRatio * tan_h), 0, 0, 0,
        0, 1.0 / (m_far * tan_h), 0, 0,
        0, 0, 1.0 / m_far, 0,
        0, 0, 0, 1
    ));
}

void CamtransCamera::updateViewMatrix(){
    updateTranslationMatrix();
    updateRotationMatrix();
}

void CamtransCamera::updateRotationMatrix(){
    m_rotationMatrix = glm::transpose(glm::mat4(
        m_u.x, m_u.y, m_u.z, 0,
        m_v.x, m_v.y, m_v.z, 0,
        m_w.x, m_w.y, m_w.z, 0,
        0,     0,     0,     1
    ));
}

void CamtransCamera::updateTranslationMatrix(){
    m_translationMatrix = glm::transpose(glm::mat4(
        1, 0, 0, -m_eye.x,
        0, 1, 0, -m_eye.y,
        0, 0, 1, -m_eye.z,
        0, 0, 0, 1
    ));
}

// @TODO Task 2: Fill in the helper methods you created for updating the matrices...
