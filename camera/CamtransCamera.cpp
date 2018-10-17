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

glm::mat4x4 CamtransCamera::getProjectionMatrix() const {
    // TODO Task 3: Camtrans
}

glm::mat4x4 CamtransCamera::getViewMatrix() const {
    // TODO Task 3: Camtrans
}

glm::mat4x4 CamtransCamera::getScaleMatrix() const {
    // TODO Task 1: Camtrans
}

glm::mat4x4 CamtransCamera::getPerspectiveMatrix() const {
    // TODO Task 1: Camtrans
}

glm::vec4 CamtransCamera::getPosition() const {
    // TODO Task 1: Camtrans
}

glm::vec4 CamtransCamera::getU() const {
    // TODO Task 1: Camtrans
}

glm::vec4 CamtransCamera::getV() const {
    // TODO Task 1: Camtrans
}

glm::vec4 CamtransCamera::getW() const {
    // TODO Task 1: Camtrans
}

glm::vec4 CamtransCamera::getLook() const {
    // TODO Task 1: Camtrans
}

float CamtransCamera::getAspectRatio() const {
    // TODO Task 1: Camtrans
}

float CamtransCamera::getHeightAngle() const {
    // TODO Task 1: Camtrans
}

void CamtransCamera::orientLook(const glm::vec4 &eye, const glm::vec4 &look, const glm::vec4 &up) {
  // TODO Task 3: Camtrans
}

void CamtransCamera::setHeightAngle(float h) {
    // TODO Task 3: Camtrans
}

void CamtransCamera::setAspectRatio(float a) {
    // TODO Task 3: Camtrans
}

void CamtransCamera::translate(const glm::vec4 &v) {
    // TODO: Camtrans
}

void CamtransCamera::rotateU(float degrees) {
    // TODO Task 3: Camtrans
}

void CamtransCamera::rotateV(float degrees) {
    // TODO Task 3: Camtrans
}

void CamtransCamera::rotateW(float degrees) {
    // TODO Task 3: Camtrans
}

void CamtransCamera::setClip(float nearPlane, float farPlane) {
    // TODO Task 3: Camtrans
}

// @TODO Task 1: Define the helper methods for updating the matrices here...
// @TODO Task 2: Fill in the helper methods you created for updating the matrices...
