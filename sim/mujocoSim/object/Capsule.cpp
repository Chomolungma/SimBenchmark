//
// Created by kangd on 19.02.18.
//

#include "Capsule.hpp"

mujoco_sim::object::Capsule::Capsule(double radius, double height, mjData *data, mjModel *model, int objectID)
    : SingleBodyObject(data, model, objectID) {}
