//
// Created by kangd on 25.03.18.
//

#ifndef BULLETSIM_ARTICULATEDSYSTEM_HPP
#define BULLETSIM_ARTICULATEDSYSTEM_HPP

#include <string>
#include <btBulletCollisionCommon.h>
#include <BulletDynamics/Featherstone/btMultiBodyDynamicsWorld.h>
#include <BulletDynamics/Featherstone/btMultiBody.h>
#include <BulletDynamics/Featherstone/btMultiBodyLink.h>

#include "URDF/BulletUrdfImporter.h"
#include "URDF/MyMultiBodyCreator.h"
#include "URDF/URDFToBullet.h"
#include "URDF/UrdfParser.h"

#include "common/ArticulatedSystem.hpp"
#include "bulletSim/object/ArticulatedSystem/URDF/URDFToBullet.h"
#include "bulletSim/object/Object.hpp"

namespace bullet_sim {
namespace object {

class ArticulatedSystem: public Object, public benchmark::object::ArticulatedSystem {

 public:
  ArticulatedSystem(std::string urdfFile, btMultiBodyDynamicsWorld *world);
  virtual ~ArticulatedSystem();

  void updateVisuals();

  virtual const EigenVec getGeneralizedCoordinate() override;

  virtual const EigenVec getGeneralizedVelocity() override;

  /* For floating-base robots, [linearPosition_W, baseRationInQuaternion, joint Angles]
   * For fixed-base robot, [joint angles]
   * The dimension is the DOF+1 for floating-based, and DOF for fixed based. (obtained by getDOF())*/
  virtual void setGeneralizedCoordinate(const Eigen::VectorXd &jointState) override;

  /* For floating-base robots, [linearVelocity_W, angularVelocity_W, jointVelocity]
   * The dimension is the same as dof (obtained with getDOF)*/
  virtual void setGeneralizedVelocity(const Eigen::VectorXd &jointVel) override;

  virtual void setGeneralizedCoordinate(std::initializer_list<double> jointState) override;

  virtual void setGeneralizedVelocity(std::initializer_list<double> jointVel) override;

  virtual void setGeneralizedForce(std::initializer_list<double> tau) override;

  virtual void getState(Eigen::VectorXd &genco, Eigen::VectorXd &genvel) override;

  virtual void setState(const Eigen::VectorXd &genco, const Eigen::VectorXd &genvel) override;

  virtual void setGeneralizedForce(const Eigen::VectorXd &tau) override;

  virtual const EigenVec getGeneralizedForce() override;

  virtual int getDOF() override ;

 private:
  void init();
  void initVisuals();

  void initVisObj(btTransform linkTransform, std::vector<URDFVisualData> &data);
  void initVisColObjFromLinkCollider(btMultiBodyLinkCollider *linkCollider, int colliderId);
  void initVisColObjFromCompoundChildList(btCompoundShapeChild *compoundShapeChild,
                                          btQuaternion parentQuat,
                                          btVector3 parentPos,
                                          int id,
                                          int numChild);
  void initVisColObjFromCollisionShape(btCollisionShape *collisionShape,
                                       btQuaternion quat,
                                       btVector3 pos,
                                       int id);

  btMultiBodyDynamicsWorld *dynamicsWorld_;
  btMultiBody *multiBody_;
  BulletURDFImporter *importer_;
  MyMultiBodyCreator *creator_;

  // generalized states, velocity, force
  rai_sim::VecDyn jointState_;
  rai_sim::VecDyn jointVel_;
  rai_sim::VecDyn jointForce_;

  std::vector<int> movableLinkIdx_;

  double maxJointTorque_ = 1000.0;

  int dof_ = 0;
  int stateDimension_ = 0;

  bool isFixed_ = true;
};

} // object
} // rai_sim

#endif //BULLETSIM_ARTICULATEDSYSTEM_HPP
