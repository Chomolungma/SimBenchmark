//
// Created by kangd on 17.04.18.
//

#ifndef BENCHMARK_ODELINKJOINT_HPP
#define BENCHMARK_ODELINKJOINT_HPP

#include <ode/ode.h>
#include "common/math.hpp"

namespace bo = benchmark::object;
namespace ode_sim {
namespace object {

// world pos, world orientation, parent id, shape, color
typedef std::tuple<benchmark::Mat<3, 3>,
                   benchmark::Vec<3>,
                   int,
                   bo::Shape,
                   benchmark::Vec<4>> VisualObjectData;
// mesh path, size
typedef std::pair<std::string,
                  benchmark::Vec<4>> VisualObjectProperty;
// ...
typedef std::tuple<benchmark::Mat<3, 3>,
                   benchmark::Vec<3>,
                   int,
                   bo::Shape> AlternativeVisualObjectData;


struct Joint {

  enum Type {
    FIXED,
    REVOLUTE,
    PRISMATIC,
    FLOATING
  };

  void jointAxis(std::initializer_list<double> a) {
    axis_[0] = *(a.begin());
    axis_[1] = *(a.begin() + 1);
    axis_[2] = *(a.begin() + 2);
  }

  void jointPosition(std::initializer_list<double> p) {
    pos_[0] = *(p.begin());
    pos_[1] = *(p.begin() + 1);
    pos_[2] = *(p.begin() + 2);
  }

  benchmark::Vec<3> axis_;
  benchmark::Vec<3> pos_;
  benchmark::Mat<3,3> rotmat_;
  Type type;

  // ode
  dJointID odeJoint_ = 0;
};


struct LinkInertial {
  LinkInertial() {
    mass_ = 0;
    memset(inertia_.ptr(), 0, 9 * sizeof(double));
    memset(pos_.ptr(), 0, 3 * sizeof(double));
    memset(rotmat_.ptr(), 0, 9 * sizeof(double));
    rotmat_[0] = 1;
    rotmat_[4] = 1;
    rotmat_[8] = 1;
  }
  benchmark::Mat<3, 3> inertia_;
  benchmark::Mat<3, 3> rotmat_;   // w.r.t joint
  benchmark::Vec<3> pos_;    // w.r.t joint
  double mass_;
};


struct LinkVisual {
  void visOrientation(const benchmark::Vec<3> &rpy) {
    visObjRotMat_.emplace_back();
    benchmark::rpyToRotMat_intrinsic(rpy, visObjRotMat_.back());
  }

  std::vector<bo::Shape> visshape_;
  std::vector<benchmark::Vec<4>> visShapeParam_;
  std::vector<benchmark::Vec<3>> visObjOrigin_;     // w.r.t joint
  std::vector<benchmark::Mat<3, 3>> visObjRotMat_;  // w.r.t joint
  std::vector<benchmark::Vec<4>> visColor_;
  std::vector<std::string> meshFileNames_;
};


struct LinkCollision {
  void colOrientation(const benchmark::Vec<3> &rpy) {
    colObjRotMat_.emplace_back();
    benchmark::rpyToRotMat_intrinsic(rpy, colObjRotMat_.back());
  }

  std::vector<bo::Shape> colShape_;
  std::vector<benchmark::Vec<4>> colShapeParam_;
  std::vector<benchmark::Vec<3>> colObjOrigin_;     // w.r.t joint
  std::vector<benchmark::Mat<3, 3>> colObjRotMat_;  // w.r.t joint
};


struct Link {

  std::vector<Link> childrenLinks_;

  LinkInertial inertial_;

  LinkCollision collision_;

  LinkVisual visual_;

  Joint parentJoint_;

  std::string name_;
  std::string parentName_;
  std::string parentJointName_;

  int bodyIdx_;
  int parentIdx_;


  // relative com position and body orientation w.r.t parent
  benchmark::Vec<3> com_B_;
  benchmark::Mat<3, 3> rotMat_B_;

  /// collision ode
  dBodyID odeBody_ = 0;
  dMass odeMass_;
  std::vector<dGeomID> odeGeometries_;
//  std::vector<MetrialProp *> matrialProps_;


  void RPY(const benchmark::Vec<3> &rpy) {
    rpyToRotMat_intrinsic(rpy, rotMat_B_);
  }

//  void initInertial() {
////  odeBody_ = dBodyCreate(worldID_);
//
//      if(mass_ == 0) {
//        inertia_.setZero();
//      } else {
//        // TODO cgx, cgy, cgz check
//        dMassSetParameters(
//            &odeMass_,
//            mass_,
//            com_B_[0],
//            com_B_[1],
//            com_B_[2],
//            inertia_[0],
//            inertia_[4],
//            inertia_[8],
//            inertia_[1],
//            inertia_[2],
//            inertia_[7]
//        );
//        dBodySetMass(odeBody_, &odeMass_);
//      }
//
//      for (auto &ch: childrenLinks_)
//        ch.initInertial();
//  }
//
//  void initCollisions(std::vector<AlternativeVisualObjectData> &collect,
//                      std::vector<VisualObjectProperty> &props);

  int numberOfBodiesFromHere() {
    int nbody = 1;
    for (auto &ch : childrenLinks_)
      nbody += ch.numberOfBodiesFromHere();
    return nbody;
  }

  int jointIdx(std::string &nm, std::vector<std::string> &jointsNames) {
    for (uint i = 0; i < jointsNames.size(); i++)
      if (nm == jointsNames[i]) return int(i);
    return -1;
  }
};

}
}

#endif //BENCHMARK_ODELINKJOINT_HPP
