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
#include <BulletDynamics/Featherstone/btMultiBodyLinkCollider.h>

#include "common/interface/ArticulatedSystemInterface.hpp"

#include "URDF/BulletUrdfImporter.h"
#include "URDF/MyMultiBodyCreator.h"
#include "URDF/URDFToBullet.h"
#include "URDF/UrdfParser.h"

#include "object/ArticulatedSystem/URDF/URDFToBullet.h"
#include "object/BtObject.hpp"

namespace bullet_sim {
namespace object {

class BtArticulatedSystem: public bullet_sim::object::BtObject,
                         public benchmark::object::ArticulatedSystemInterface {

 public:
  BtArticulatedSystem(std::string urdfFile, btMultiBodyDynamicsWorld *world);
  virtual ~BtArticulatedSystem();

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
  
  virtual int getStateDimension() override ; 

  virtual void setColor(Eigen::Vector4d color) override ;

  void getBodyPose(int bodyId,
                   benchmark::Mat<3, 3> &orientation,
                   benchmark::Vec<3> &position);

 private:
  void init();
  void initVisuals();
  void initVisualFromLinkCollider(btMultiBodyLinkCollider *linkCollider, int colliderId);
  void initVisualFromCompoundChildList(btCompoundShapeChild *compoundShapeChild,
                                         int id,
                                         int numChild);
  void initVisualFromCollisionShape(btCollisionShape *collisionShape,
                                    btTransform transform,
                                    int id);
  void initVisualFromVisualShape(int id);

  btMultiBodyDynamicsWorld *dynamicsWorld_;
  btMultiBody *multiBody_;
  BulletURDFImporter *importer_;

  std::vector<int> movableLinkIdx_;
  double maxJointTorque_ = 1000.0;
};

} // object
} // rai_sim

#endif //BULLETSIM_ARTICULATEDSYSTEM_HPP
