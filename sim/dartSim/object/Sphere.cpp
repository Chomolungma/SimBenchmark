//
// Created by kangd on 19.03.18.
//

#include "Sphere.hpp"

namespace dart_sim {
namespace object {

Sphere::Sphere(double radius, double mass): SingleBodyObject(mass) {

  skeletonPtr_ = dart::dynamics::Skeleton::create();
  shapePtr_ = dart::dynamics::ShapePtr(new dart::dynamics::SphereShape(radius));

  dart::dynamics::BodyNode::Properties bodyProp;
  bodyProp.mName = "sphere_link";
  bodyProp.mInertia.setMass(mass);

  Eigen::Isometry3d T = Eigen::Isometry3d::Identity();

  dart::dynamics::FreeJoint::Properties jointProp;
  jointProp.mName = "sphere_joint";
  jointProp.mT_ParentBodyToJoint = T;

  auto pair = skeletonPtr_->createJointAndBodyNodePair<dart::dynamics::FreeJoint>(
      nullptr, jointProp, bodyProp);
  auto shapeNode = pair.second->createShapeNodeWith<
      dart::dynamics::CollisionAspect,
      dart::dynamics::DynamicsAspect>(shapePtr_);

}

} // object
} // dart_sim