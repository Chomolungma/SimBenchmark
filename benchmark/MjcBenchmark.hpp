//
// Created by kangd on 27.04.18.
//

#ifndef BENCHMARK_DARTBENCHMARK_HPP
#define BENCHMARK_DARTBENCHMARK_HPP

#include <raiCommon/rai_utils.hpp>
#include <boost/program_options.hpp>
#include "mujocoSim/src/MjcWorld.hpp"

namespace po = boost::program_options;

namespace benchmark::mujoco {

/**
 * options for Dart simulation
 */
struct Option {
  mujoco_sim::SolverOption solverOption = mujoco_sim::SOLVER_PGS;
};
Option options;

std::string getKeypath() {

  std::string keyPath(__FILE__);
  while (keyPath.back() != '/')
    keyPath.erase(keyPath.size() - 1, 1);
  keyPath += "../lib/mjpro150/mjkey.txt";

  return keyPath;
}

/**
 * add options to desc
 *
 * @param desc
 */
void addDescToOption(po::options_description &desc) {
  desc.add_options()
      ("solver", po::value<std::string>(), "constraint solver type (pgs / cg / newton)")
      ;
}

/**
 * get option or parameter from arguments
 *
 * @param argc
 * @param argv
 * @param desc
 */
void getParamsFromArg(int argc, const char *argv[], po::options_description &desc) {

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);

  // help option
  if(vm.count("solver")) {
    if(vm["solver"].as<std::string>().compare("pgs") == 0) {
      options.solverOption = mujoco_sim::SOLVER_PGS;
    }
    else if (vm["solver"].as<std::string>().compare("cg") == 0) {
      options.solverOption = mujoco_sim::SOLVER_CG;
    }
    else if (vm["solver"].as<std::string>().compare("newton") == 0) {
      options.solverOption = mujoco_sim::SOLVER_NEWTON;
    }
    else {
      RAIFATAL("invalid solver input")
    }
  }
}

} // benchmark::ode

#endif //BENCHMARK_DARTBENCHMARK_HPP
