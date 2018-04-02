//
// Created by kangd on 18.02.18.
//

#ifndef BENCHMARK_WORLD_HPP
#define BENCHMARK_WORLD_HPP

#include <mujoco.h>

#include "common/Configure.hpp"
#include "common/WorldInterface.hpp"

#include "object/Sphere.hpp"
#include "object/Box.hpp"
#include "object/Capsule.hpp"
#include "object/CheckerBoard.hpp"
#include "object/Cylinder.hpp"

namespace mujoco_sim {

enum SolverOption {
  SOLVER_PGS,
  SOLVER_CG,
  SOLVER_NEWTON
};

class World: public benchmark::WorldInterface {

 public:
  World(const char *modelPath,
          const char *keyPath,
          SolverOption solverOption);
  virtual ~World();

  object::Sphere *addSphere(double radius,
                              double mass,
                              benchmark::CollisionGroupType collisionGroup,
                              benchmark::CollisionGroupType collisionMask) override;
  object::Box *addBox(double xLength,
                        double yLength,
                        double zLength,
                        double mass,
                        benchmark::CollisionGroupType collisionGroup,
                        benchmark::CollisionGroupType collisionMask) override;
  object::CheckerBoard *addCheckerboard(double gridSize,
                                          double xLength,
                                          double yLength,
                                          double reflectanceI,
                                          benchmark::CollisionGroupType collisionGroup,
                                          benchmark::CollisionGroupType collisionMask) override;
  object::Capsule *addCapsule(double radius,
                              double height,
                              double mass,
                              benchmark::CollisionGroupType collisionGroup,
                              benchmark::CollisionGroupType collisionMask) override ;
  object::Cylinder *addCylinder(double radius,
                                double height,
                                double mass,
                                benchmark::CollisionGroupType collisionGroup=1,
                                benchmark::CollisionGroupType collisionMask=-1) override ;

  mjModel *getWorldModel() const;
  mjData *getWorldData() const;

  void integrate(double dt);

 private:
  mjModel *worldModel_;
  mjData *worldData_;

  // list
  std::vector<object::SingleBodyObject*> objectList_;

};

} // mujoco_sim

#endif //BENCHMARK_WORLD_HPP
