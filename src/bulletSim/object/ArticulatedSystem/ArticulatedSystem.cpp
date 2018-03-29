//
// Created by kangd on 25.03.18.
//

#include <raiSim/math.hpp>
#include <BulletDynamics/Featherstone/btMultiBodyLinkCollider.h>
#include <SharedMemoryPublic.h>
#include "ArticulatedSystem.hpp"

namespace bullet_sim {
namespace object {

ArticulatedSystem::ArticulatedSystem(std::string urdfFile, btMultiBodyDynamicsWorld *world) {

  BulletURDFImporter importer(0, 0, 1.0, CUF_USE_IMPLICIT_CYLINDER | CUF_USE_URDF_INERTIA);
  bool loadOK = importer.loadURDF(urdfFile.c_str());

  if(loadOK) {
    MyMultiBodyCreator creator(0);

    btTransform identityTrans;
    identityTrans.setIdentity();

    ConvertURDF2Bullet2(importer, creator, identityTrans, world, true, importer.getPathPrefix());

    multiBody_ = creator.getBulletMultiBody();
    init();
  }
  else {
    RAIFATAL("failed to load URDF")
  }
}

ArticulatedSystem::~ArticulatedSystem() {
}

void ArticulatedSystem::init() {

  if(multiBody_->hasFixedBase()) {
    isFixed_ = true;
    dof_ = multiBody_->getNumDofs();
    stateDimension_ = dof_;
  }
  else {
    isFixed_ = false;
    dof_ = multiBody_->getNumDofs();
    stateDimension_ = dof_ + 7;
  }

  jointState_.resize(stateDimension_);
  jointState_.setZero();
  jointVel_.resize(dof_);
  jointVel_.setZero();

  // find movable links
  movableLinkIdx_.reserve(multiBody_->getNumDofs());

  for (int i = 0; i < multiBody_->getNumLinks(); i++) {
    switch(multiBody_->getLink(i).m_jointType) {
      case btMultibodyLink::eRevolute: {
        // 1 DOF
        movableLinkIdx_.push_back(i);
        break;
      }
      case btMultibodyLink::ePrismatic: {
        // 1 DOF
        movableLinkIdx_.push_back(i);
        break;
      }
      case btMultibodyLink::eSpherical: {
        RAIFATAL("spherical joint is not supported")
        break;
      }
      case btMultibodyLink::ePlanar: {
        RAIFATAL("planar joint is not supported")
        break;
      }
      case btMultibodyLink::eFixed:
        // do nothing
        break;
      case btMultibodyLink::eInvalid: {
        RAIFATAL("invalid joint type")
        break;
      }
      default: {
        RAIFATAL("invalid joint type")
        break;
      }
    }
  }

  initVisuals();
}

void ArticulatedSystem::updateVisuals() {
  visObj.clear();
  visColObj.clear();
  visProps_.clear();
  visColProps_.clear();

  initVisuals();
}

void ArticulatedSystem::initVisuals() {

  // reserve
  visObj.reserve(multiBody_->getNumLinks() + 1);
  visColObj.reserve(multiBody_->getNumLinks() + 1);
  visProps_.reserve(multiBody_->getNumLinks() + 1);
  visColProps_.reserve(multiBody_->getNumLinks() + 1);

  // base
  {
    btMultiBodyLinkCollider *baseCollider = multiBody_->getBaseCollider();
    initVisualFromLinkCollider(baseCollider, 0);
  }

  // link
  for (int i = 0; i < multiBody_->getNumLinks(); i++) {
    btMultiBodyLinkCollider *linkCollider = multiBody_->getLinkCollider(i);
    initVisualFromLinkCollider(linkCollider, i + 1);
  }
}

void ArticulatedSystem::initVisualFromLinkCollider(btMultiBodyLinkCollider *linkCollider, int colliderId) {

  // shape
  if (linkCollider->getCollisionShape()->isCompound()) {

    // compound shape
    btCompoundShape *compoundShape =
        (btCompoundShape *) linkCollider->getCollisionShape();

    if(compoundShape->getNumChildShapes() > 0) {
      initVisualFromCompoundChildList(compoundShape->getChildList(),
                                      linkCollider->getWorldTransform().getRotation(),
                                      linkCollider->getWorldTransform().getOrigin(),
                                      colliderId,
                                      compoundShape->getNumChildShapes());
    }
//    else {
//      initVisualFromCollisionShape(linkCollider->getCollisionShape(),
//                                   linkCollider->getWorldTransform().getRotation(),
//                                   linkCollider->getWorldTransform().getOrigin(),
//                                   colliderId);
//    }
  }
  else {

    // single shape
    initVisualFromCollisionShape(linkCollider->getCollisionShape(),
                                 linkCollider->getWorldTransform().getRotation(),
                                 linkCollider->getWorldTransform().getOrigin(),
                                 colliderId);
  }
}
void ArticulatedSystem::initVisualFromCompoundChildList(btCompoundShapeChild *compoundShapeChild,
                                                        btQuaternion parentQuat,
                                                        btVector3 parentPos,
                                                        int id,
                                                        int numChild) {
  for (int i = 0; i < numChild; ++i) {
    btQuaternion childquat = parentQuat * compoundShapeChild[i].m_transform.getRotation();
    btVector3 childpos = parentPos + compoundShapeChild[i].m_transform.getOrigin();
    initVisualFromCollisionShape(compoundShapeChild[i].m_childShape, childquat, childpos, id);
  }
}
void ArticulatedSystem::initVisualFromCollisionShape(btCollisionShape *col,
                                                     btQuaternion quat,
                                                     btVector3 pos,
                                                     int id) {

  // orientation
  rai_sim::Mat<3, 3> mat;
  btMatrix3x3 rotMat;
  rotMat.setRotation(quat);
  mat.e() << rotMat.getRow(0).x(), rotMat.getRow(0).y(), rotMat.getRow(0).z(),
      rotMat.getRow(1).x(), rotMat.getRow(1).y(), rotMat.getRow(1).z(),
      rotMat.getRow(2).x(), rotMat.getRow(2).y(), rotMat.getRow(2).z();

  // position
  rai_sim::Vec<3> position;
  position = {pos.x(), pos.y(), pos.z()};

  // color
  rai_sim::Vec<4> color;
  color = {1.0, 0, 0, 1.0};

  switch (col->getShapeType()) {
    case BOX_SHAPE_PROXYTYPE: {
      // box (xlen, ylen, zlen)
      rai_sim::Vec<4> boxSize;
      boxSize = {((btBoxShape *)col)->getHalfExtentsWithMargin().x() * 2.0,
                 ((btBoxShape *)col)->getHalfExtentsWithMargin().y() * 2.0,
                 ((btBoxShape *)col)->getHalfExtentsWithMargin().z() * 2.0,
                 0};

      visObj.emplace_back(std::make_tuple(mat, position, id, benchmark::object::Shape::Box, color));
      visColObj.emplace_back(std::make_tuple(mat, position, id, benchmark::object::Shape::Box));
      visProps_.emplace_back(std::make_pair("", boxSize));
      visColProps_.emplace_back(std::make_pair("", boxSize));
      break;
    }
    case CYLINDER_SHAPE_PROXYTYPE: {
      // cylinder (rad, rad, height)
      rai_sim::Vec<4> cylSize;
      cylSize = {((btCylinderShapeZ *)col)->getHalfExtentsWithMargin().x(),
                 ((btCylinderShapeZ *)col)->getHalfExtentsWithMargin().z() * 2.0,
                 0,
                 0};

      visObj.emplace_back(std::make_tuple(mat, position, id, benchmark::object::Shape::Cylinder, color));
      visColObj.emplace_back(std::make_tuple(mat, position, id, benchmark::object::Shape::Cylinder));
      visProps_.emplace_back(std::make_pair("", cylSize));
      visColProps_.emplace_back(std::make_pair("", cylSize));
      break;
    }
    case SPHERE_SHAPE_PROXYTYPE: {
      // sphere
      rai_sim::Vec<4> sphereSize;
      sphereSize = {((btSphereShape *)col)->getRadius(),
                    0,
                    0,
                    0};

      visObj.emplace_back(std::make_tuple(mat, position, id, benchmark::object::Shape::Sphere, color));
      visColObj.emplace_back(std::make_tuple(mat, position, id, benchmark::object::Shape::Sphere));
      visProps_.emplace_back(std::make_pair("", sphereSize));
      visColProps_.emplace_back(std::make_pair("", sphereSize));
      break;
    }
    default:
      break;
  }
}
const ArticulatedSystem::EigenVec ArticulatedSystem::getGeneralizedCoordinate() {
  if (isFixed_) {
    // fixed body
    int i = 0;
    for (int l: movableLinkIdx_) {
      jointState_[i++] = multiBody_->getJointPos(l);
    }
  }
  else {
    // floating body
    jointState_[0] = multiBody_->getBaseWorldTransform().getOrigin().x();
    jointState_[1] = multiBody_->getBaseWorldTransform().getOrigin().y();
    jointState_[2] = multiBody_->getBaseWorldTransform().getOrigin().z();
    jointState_[3] = multiBody_->getBaseWorldTransform().getRotation().w();
    jointState_[4] = multiBody_->getBaseWorldTransform().getRotation().x();
    jointState_[5] = multiBody_->getBaseWorldTransform().getRotation().y();
    jointState_[6] = multiBody_->getBaseWorldTransform().getRotation().z();

    int i = 7;
    for (int l: movableLinkIdx_) {
      jointState_[i++] = multiBody_->getJointPos(l);
    }
  }
  return jointState_.e();
}
const ArticulatedSystem::EigenVec ArticulatedSystem::getGeneralizedVelocity() {
  if (multiBody_->hasFixedBase()) {
    // fixed body
    // TODO
    RAIFATAL("fixed body is not yet implemented")
  } else {
    // floating body
    const int numStates = multiBody_->getNumLinks() + 7;
    rai_sim::VecDyn velocity;
    velocity.resize(numStates);

    velocity[0] = multiBody_->getBaseVel().x();
    velocity[1] = multiBody_->getBaseVel().y();
    velocity[2] = multiBody_->getBaseVel().z();
    velocity[3] = multiBody_->getBaseOmega().x();
    velocity[4] = multiBody_->getBaseOmega().x();
    velocity[5] = multiBody_->getBaseOmega().y();
    velocity[6] = multiBody_->getBaseOmega().z();

    for (int i = 0; i < multiBody_->getNumLinks(); i++) {
      velocity[i+7] = multiBody_->getJointPos(i);
    }

    return velocity.e();
  }
}

void ArticulatedSystem::setGeneralizedCoordinate(const Eigen::VectorXd &jointState) {
  if(isFixed_) {
    // fixed
    int i = 0;
    for (int l: movableLinkIdx_) {
      multiBody_->setJointPos(l, jointState[i++]);
    }
  } else {
    // floating
    multiBody_->getBaseWorldTransform().setOrigin(btVector3(jointState[0], jointState[1], jointState[2]));
    multiBody_->getBaseWorldTransform().setRotation(btQuaternion(jointState[3], jointState[4], jointState[5], jointState[6]));

    int i = 7;
    for (int l: movableLinkIdx_) {
      multiBody_->setJointPos(l, jointState[i++]);
    }
  }
}
void ArticulatedSystem::setGeneralizedVelocity(const Eigen::VectorXd &jointVel) {

}
void ArticulatedSystem::setGeneralizedCoordinate(std::initializer_list<double> jointState) {
  if(isFixed_) {
    // fixed
    int i = 0;
    for (int l: movableLinkIdx_) {
      multiBody_->setJointPos(l, jointState.begin()[i]);
    }
  } else {
    // floating
    multiBody_->setBasePos(btVector3(
        jointState.begin()[0],
        jointState.begin()[1],
        jointState.begin()[2]));
    multiBody_->setWorldToBaseRot(btQuaternion(
        jointState.begin()[3],
        jointState.begin()[4],
        jointState.begin()[5],
        jointState.begin()[6]));

    int i = 7;
    for (int l: movableLinkIdx_) {
      multiBody_->setJointPos(l, jointState.begin()[i]);
    }
  }
}
void ArticulatedSystem::setGeneralizedVelocity(std::initializer_list<double> jointVel) {

}
const ArticulatedSystem::EigenVec ArticulatedSystem::getGeneralizedForce() {
  multiBody_->getBaseForce();
  rai_sim::VecDyn force;
  return force.e();
}
void ArticulatedSystem::setGeneralizedForce(std::initializer_list<double> tau) {

}
void ArticulatedSystem::getState(Eigen::VectorXd &genco, Eigen::VectorXd &genvel) {

}
void ArticulatedSystem::setState(const Eigen::VectorXd &genco, const Eigen::VectorXd &genvel) {

}
void ArticulatedSystem::setGeneralizedForce(const Eigen::VectorXd &tau) {

}

} // object
} // bulet_sim
