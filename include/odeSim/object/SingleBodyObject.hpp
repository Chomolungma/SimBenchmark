//
// Created by kangd on 11.02.18.
//

#ifndef ODESIM_SINGLEBODYOBJECT_HPP
#define ODESIM_SINGLEBODYOBJECT_HPP

#include <ode/common.h>
#include <ode/ode.h>
#include <Eigen/Geometry>
#include <raiSim/math.hpp>

namespace ode_sim {
namespace object {

class SingleBodyObject {

 public:
  SingleBodyObject(const dWorldID worldID);
  virtual ~SingleBodyObject();

  virtual const Eigen::Map<Eigen::Matrix<double, 4, 1>> getQuaternion();
  virtual void getQuaternion(rai_sim::Vec<4>& quat);
  virtual const Eigen::Map<Eigen::Matrix<double, 3, 3> > getRotationMatrix();
  virtual void getRotationMatrix(rai_sim::Mat<3,3>& rotation);
  virtual const Eigen::Map<Eigen::Matrix<double, 3, 1> > getPosition();
  virtual const Eigen::Map<Eigen::Matrix<double, 3, 1> > getComPosition();
  virtual  const Eigen::Map<Eigen::Matrix<double, 3, 1> > getLinearVelocity();
  virtual const Eigen::Map<Eigen::Matrix<double, 3, 1> > getAngularVelocity();
  virtual void getPosition_W(rai_sim::Vec<3>& pos_w);

  virtual void setPosition(Eigen::Vector3d originPosition);
  virtual void setPosition(double x, double y, double z);
  virtual void setOrientation(Eigen::Quaterniond quaternion);
  virtual void setOrientation(double w, double x, double y, double z);
  virtual void setOrientation(Eigen::Matrix3d rotationMatrix);
//  void setOrientationRandom();
  virtual void setPose(Eigen::Vector3d originPosition, Eigen::Quaterniond quaternion);
  virtual void setPose(Eigen::Vector3d originPosition, Eigen::Matrix3d rotationMatrix);
  virtual void setVelocity(Eigen::Vector3d linearVelocity, Eigen::Vector3d angularVelocity);
  virtual void setVelocity(double dx, double dy, double dz, double wx, double wy, double wz);

  virtual bool isVisualizeFramesAndCom() const;

 protected:
  dWorldID worldID_;
  dGeomID geometry_;
  dBodyID body_;
  dMass mass_;

  // from object
  bool visualizeFramesAndCom_ = true;

};

} // object
} // ode_sim

#endif //ODESIM_SINGLEBODYOBJECT_HPP
