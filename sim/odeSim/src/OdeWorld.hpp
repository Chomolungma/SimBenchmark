//
// Created by kangd on 11.02.18.
//

#ifndef ODESIM_WORLD_HPP
#define ODESIM_WORLD_HPP

#include <ode/ode.h>

#include "common/Configure.hpp"
#include "common/interface/WorldInterface.hpp"

#include "object/OdeObject.hpp"
#include "object/OdeSphere.hpp"
#include "object/OdeBox.hpp"
#include "object/OdeCapsule.hpp"
#include "object/OdeCheckerBoard.hpp"
#include "object/OdeCylinder.hpp"
#include "object/OdeArticulatedSystem.hpp"

namespace ode_sim {

enum SolverOption {
  SOLVER_STANDARD,
  SOLVER_QUICK
};

class OdeWorld: public benchmark::WorldInterface {

 public:
  explicit OdeWorld(SolverOption solverOption = SOLVER_STANDARD);
  virtual ~OdeWorld();

  object::OdeSphere *addSphere(double radius,
                               double mass,
                               benchmark::CollisionGroupType collisionGroup=1,
                               benchmark::CollisionGroupType collisionMask=-1) override ;

  object::OdeBox *addBox(double xLength,
                         double yLength,
                         double zLength,
                         double mass,
                         benchmark::CollisionGroupType collisionGroup=1,
                         benchmark::CollisionGroupType collisionMask=-1) override;

  object::OdeCheckerBoard *addCheckerboard(double gridSize,
                                           double xLength,
                                           double yLength,
                                           double reflectanceI,
                                           bo::CheckerboardShape shape,
                                           benchmark::CollisionGroupType collisionGroup,
                                           benchmark::CollisionGroupType collisionMask) override;

  object::OdeCapsule *addCapsule(double radius,
                                 double height,
                                 double mass,
                                 benchmark::CollisionGroupType collisionGroup=1,
                                 benchmark::CollisionGroupType collisionMask=-1) override;

  object::OdeCylinder *addCylinder(double radius,
                                   double height,
                                   double mass,
                                   benchmark::CollisionGroupType collisionGroup=1,
                                   benchmark::CollisionGroupType collisionMask=-1) override ;

  object::OdeArticulatedSystem *addArticulatedSystem(std::string urdfPath,
                                                    benchmark::CollisionGroupType collisionGroup=1,
                                                    benchmark::CollisionGroupType collisionMask=-1);

  void integrate(double dt) override ;

  void setGravity(const benchmark::Vec<3> &gravity) override ;
  void setERP(double erp);
  virtual int getNumObject() override ;

  // dynamics world
  static dWorldID dynamicsWorld_;
  static dJointGroupID contactGroup_;

  // constants
  static const int maxContactsPerBody = 10;

 private:

  // call back
  static void nearCallback(void *data, dGeomID o1, dGeomID o2);

  // space
  dSpaceID space_;

  // simulation properties
  dVector3 gravity_ = {0, 0, -9.81};

  // contact solver
  SolverOption solverOption_ = SOLVER_STANDARD;

  // list
  std::vector<object::OdeObject*> objectList_;

};

}

#endif //ODESIM_WORLD_HPP
