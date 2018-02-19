//
// Created by kangd on 18.02.18.
//

#include "SingleBodyObject.hpp"

namespace mujoco_sim {
namespace object {

SingleBodyObject::SingleBodyObject(mjData *data, int objectID) : worldData_(data), objectID_(objectID) {}

const Eigen::Map<Eigen::Matrix<double, 4, 1>> SingleBodyObject::getQuaternion() {
  mjtNum *quaternion = (worldData_->xquat + 4 * objectID_);
  quatTemp_ = {quaternion[0], quaternion[1], quaternion[2], quaternion[3]};
  return quatTemp_.e();
}

void SingleBodyObject::getQuaternion(rai_sim::Vec<4> &quat) {
  mjtNum *quaternion = (worldData_->xquat + 4 * objectID_);
  quat = {quaternion[0], quaternion[1], quaternion[2], quaternion[3]};
}

const Eigen::Map<Eigen::Matrix<double, 3, 3> > SingleBodyObject::getRotationMatrix() {
  mjtNum *rotMat = (worldData_->xmat + 9 * objectID_);
  rotMatTemp_.e() << rotMat[0], rotMat[1], rotMat[2],
      rotMat[3], rotMat[4], rotMat[5],
      rotMat[6], rotMat[7], rotMat[8];
  return rotMatTemp_.e();
}

void SingleBodyObject::getRotationMatrix(rai_sim::Mat<3, 3> &rotation) {
  mjtNum *rotMat = (worldData_->xmat + 9 * objectID_);
  rotation.e() << rotMat[0], rotMat[1], rotMat[2],
      rotMat[3], rotMat[4], rotMat[5],
      rotMat[6], rotMat[7], rotMat[8];
}

const Eigen::Map<Eigen::Matrix<double, 3, 1> > SingleBodyObject::getPosition() {
  mjtNum *pos = (worldData_->xpos + 3 * objectID_);
  posTemp_ = {pos[0], pos[1], pos[2]};
  return posTemp_.e();
}

const Eigen::Map<Eigen::Matrix<double, 3, 1> > SingleBodyObject::getComPosition() {
  mjtNum *pos = (worldData_->xipos + 3 * objectID_);
  posTemp_ = {pos[0], pos[1], pos[2]};
  return posTemp_.e();
}

const Eigen::Map<Eigen::Matrix<double, 3, 1> > SingleBodyObject::getLinearVelocity() {
  mjtNum *linVel = (worldData_->cvel + 6 * objectID_ + 3);
  linVelTemp_ = {linVel[0], linVel[1], linVel[2]};
  return linVelTemp_.e();
}

const Eigen::Map<Eigen::Matrix<double, 3, 1> > SingleBodyObject::getAngularVelocity() {
  mjtNum *angVel = (worldData_->cvel + 6 * objectID_);
  angVelTemp_ = {angVel[0], angVel[1], angVel[2]};
  return angVelTemp_.e();
}
void SingleBodyObject::getPosition_W(rai_sim::Vec<3> &pos_w) {
  mjtNum *pos = (worldData_->xpos + 3 * objectID_);
  pos_w = {pos[0], pos[1], pos[2]};
}

} // object
} // mujoco_sim