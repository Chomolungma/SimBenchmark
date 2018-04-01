//
// Created by kangd on 16.03.18.
//

#ifndef DARTSIM_WORLD_HPP
#define DARTSIM_WORLD_HPP

#include <Configure.hpp>
#include <dartSim/object/Sphere.hpp>
#include <dartSim/object/Box.hpp>
#include <dartSim/object/CheckerBoard.hpp>
#include <dartSim/object/Capsule.hpp>

namespace dart_sim {

class World {

 public:
  explicit World();
  virtual ~World();

  object::Sphere *addSphere(double radius, double mass,
                            benchmark::CollisionGroupType collisionGroup=1, benchmark::CollisionGroupType collisionMask=-1) ;
  object::Box *addBox(double xLength, double yLength, double zLength, double mass,
                      benchmark::CollisionGroupType collisionGroup=1, benchmark::CollisionGroupType collisionMask=-1) ;
  object::CheckerBoard *addCheckerboard(double gridSize, double xLength, double yLength, double reflectanceI,
                                        benchmark::CollisionGroupType collisionGroup=1, benchmark::CollisionGroupType collisionMask=-1);
  object::Capsule *addCapsule(double radius, double height, double mass,
                              benchmark::CollisionGroupType collisionGroup=1, benchmark::CollisionGroupType collisionMask=-1);

  void integrate();

//  const std::vector<Single3DContactProblem> *getCollisionProblem() const;
  void setGravity(const Eigen::Vector3d &gravity);
  void setTimeStep(double timeStep);

//  void setERP(double erp, double erp2, double frictionErp);

 private:
  dart::simulation::WorldPtr dynamicsWorld_;

  // simulation properties
  Eigen::Vector3d gravity_ = {0, 0, -9.81};
  double timeStep_ = 0.01;

  // list
  std::vector<object::SingleBodyObject*> objectList_;

};

} // dart_sim

#endif //DARTSIM_WORLD_HPP
