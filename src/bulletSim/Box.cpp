//
// Created by kangd on 09.02.18.
//

#include <bulletSim/object/Box.hpp>

bullet_sim::object::Box::Box(double xlength, double ylength, double zlength, double mass) {

  // collision shape
  collisionShape_ = new btBoxShape(btVector3(.5 * xlength,
                                    .5 * ylength,
                                    .5 * zlength));

  // position and orientation
  btTransform transform;
  transform.setIdentity();
  transform.setOrigin(btVector3(0, 0, 0));
  motionState_ = new btDefaultMotionState(transform);

  // local inertia
  btVector3 localInertia(0, 0, 0);
  collisionShape_->calculateLocalInertia(mass, localInertia);

  // rigid body
  btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState_, collisionShape_, localInertia);
  rigidBody_ = new btRigidBody(rbInfo);
}
