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
}

glm::mat4x4 CamtransCamera::getProjectionMatrix() const {
    // TODO Task 3: Camtrans
    return m_projectionMatrix;
}

glm::mat4x4 CamtransCamera::getViewMatrix() const {
    // TODO Task 3: Camtrans
    return m_viewMatrix;
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
}

void CamtransCamera::setHeightAngle(float h){
    // TODO Task 3: Camtrans
}

void CamtransCamera::setAspectRatio(float a){
    // TODO Task 3: Camtrans
}

void CamtransCamera::translate(const glm::vec4 &v){
    // TODO: Camtrans
}

void CamtransCamera::rotateU(float degrees){
    // TODO Task 3: Camtrans
}

void CamtransCamera::rotateV(float degrees){
    // TODO Task 3: Camtrans
}

void CamtransCamera::rotateW(float degrees){
    // TODO Task 3: Camtrans
}

void CamtransCamera::setClip(float nearPlane, float farPlane){
    // TODO Task 3: Camtrans
}

// @TODO Task 1: Define the helper methods for updating the matrices here...
void CamtransCamera::updateProjectionMatrix(){
    updateScaleMatrix();
    updatePerspectiveMatrix();
}

void CamtransCamera::updatePerspectiveMatrix(){
    float c = - near / far;
    m_perspectiveTransformation = glm::transpose(glm::mat4(
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1 / (1 + c), (-c) / (1 + c),
        0, 0, -1, 0
    ));
}

void CamtransCamera::updateScaleMatrix(){
    m_scaleMatrix = glm::transpose(glm::mat4(
        1.0 / (m_far * glm::tan(glm::radian(m_thetaW) / 2.0)), 0, 0, 0,
        0, 1.0 / (m_far * glm::tan(glm::radian(m_thetaH) / 2.0)), 0, 0,
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
