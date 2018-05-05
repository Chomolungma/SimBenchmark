//
// Created by kangd on 15.02.18.
//

#include <raiSim/World_RG.hpp>
#include <valarray>

#include "RollingBenchmark.hpp"

rai_sim::World_RG *sim;
std::vector<rai_sim::SingleBodyHandle> objectList;
po::options_description desc;

void setupSimulation() {
  if (benchmark::rolling::options.gui)
    sim = new rai_sim::World_RG(800, 600, 0.5, rai_sim::NO_BACKGROUND);
  else
    sim = new rai_sim::World_RG();

  // erp
  if(benchmark::rolling::options.erpYN)
    sim->setERP(benchmark::rolling::params.erp);
  else
    sim->setERP(0);

  /// Note: set solver termination condition according to dt
  sim->setContactSolverParam(1.0, 1, 1.0, 50,
                             1e-7 * benchmark::rolling::options.dt);

  // set up logger and timer
  if(benchmark::rolling::options.log)
    benchmark::rolling::loggerSetup(
        benchmark::rolling::getLogDirpath(benchmark::rolling::options.erpYN,
                                          benchmark::rolling::options.forceDirection,
                                          "RAI",
                                          "RAI",
                                          benchmark::rolling::options.dt), "var"
    );
}

void setupWorld() {
  // materials
  rai_sim::MaterialManager materials;
  materials.setMaterialNames({"ground", "box", "ball"});
  materials.setMaterialPairProp("ground", "ball",
                                benchmark::rolling::params.groundMu * benchmark::rolling::params.ballMu,
                                0.0, 0.01);
  materials.setMaterialPairProp("ground", "box",
                                benchmark::rolling::params.groundMu * benchmark::rolling::params.boxMu,
                                0.0, 0.01);
  materials.setMaterialPairProp("ball", "box",
                                benchmark::rolling::params.ballMu * benchmark::rolling::params.boxMu,
                                0.0, 0.01);
  sim->updateMaterialProp(materials);

  // add objects
  auto checkerboard = sim->addCheckerboard(5.0, 100.0, 100.0, 0.1);
  checkerboard->setMaterial(sim->getMaterialKey("ground"));

  auto box = sim->addBox(20, 20, 1, 10);
  box->setPosition(0, 0, 0.5 - benchmark::rolling::params.initPenetration);
  box->setMaterial(sim->getMaterialKey("box"));
  objectList.push_back(box);

  for(int i = 0; i < 5; i++) {
    for(int j = 0; j < 5; j++) {
      auto ball = sim->addSphere(0.5, 1);
      ball->setPosition(i * 2.0 - 4.0,
                        j * 2.0 - 4.0,
                        1.5 - 3 * benchmark::rolling::params.initPenetration);
      ball->setMaterial(sim->getMaterialKey("ball"));
      objectList.push_back(ball);
    }
  }

  // gravity
  sim->setGravity({0, 0, benchmark::rolling::params.g});

  if(benchmark::rolling::options.gui) {
    sim->setLightPosition((float)benchmark::rolling::params.lightPosition[0],
                          (float)benchmark::rolling::params.lightPosition[1],
                          (float)benchmark::rolling::params.lightPosition[2]);
    sim->cameraFollowObject(checkerboard, {30, 0, 15});
  }
}

void simulationLoop() {

  // force
  rai_sim::Vec<3> force;
  if(benchmark::rolling::options.forceDirection == benchmark::rolling::FORCE_Y)
    force = {0,
             benchmark::rolling::params.F,
             0};
  else if(benchmark::rolling::options.forceDirection == benchmark::rolling::FORCE_XY)
    force = {benchmark::rolling::params.F * 0.707106781186547,
             benchmark::rolling::params.F * 0.707106781186547,
             0};

  if(benchmark::rolling::options.gui) {
    // gui
    for(int i = 0; i < (int) (benchmark::rolling::params.T / benchmark::rolling::options.dt) &&
        sim->visualizerLoop(benchmark::rolling::options.dt); i++) {

      // set force to box
      objectList[0]->setExternalForce(force, 0);

      // log
      if(benchmark::rolling::options.log) {
        ru::logger->appendData("velbox", objectList[0]->getLinearVelocity().data());
        ru::logger->appendData("velball", objectList[1]->getLinearVelocity().data());
        ru::logger->appendData("posbox", objectList[0]->getPosition().data());
        ru::logger->appendData("posball", objectList[1]->getPosition().data());
      }

      sim->integrate(benchmark::rolling::options.dt);
    }
  }
  else {
    // no gui
    if(benchmark::rolling::options.log)
      ru::timer->startTimer("rolling");

    for(int i = 0; i < (int) (benchmark::rolling::params.T / benchmark::rolling::options.dt); i++) {

      // set force to box
      objectList[0]->setExternalForce(force, 0);

      // log
      if(benchmark::rolling::options.log) {
        ru::logger->appendData("velbox", objectList[0]->getLinearVelocity().data());
        ru::logger->appendData("velball", objectList[1]->getLinearVelocity().data());
        ru::logger->appendData("posbox", objectList[0]->getPosition().data());
        ru::logger->appendData("posball", objectList[1]->getPosition().data());
      }

      sim->integrate(benchmark::rolling::options.dt);
    }

    if(benchmark::rolling::options.log)
      ru::timer->stopTimer("rolling");
  }
}

int main(int argc, const char* argv[]) {

  benchmark::rolling::addDescToOption(desc);
  benchmark::rolling::getOptionsFromArg(argc, argv, desc);
  benchmark::rolling::getParamsFromYAML(benchmark::rolling::getYamlpath().c_str(),
                                        benchmark::RAI);

  RAIINFO(
      std::endl << "=======================" << std::endl
                << "Simulator: RAI" << std::endl
                << "GUI      : " << benchmark::rolling::options.gui << std::endl
                << "ERP      : " << benchmark::rolling::options.erpYN << std::endl
                << "Force    : " << benchmark::rolling::options.forceDirection << std::endl
                << "Timestep : " << benchmark::rolling::options.dt << std::endl
                << "-----------------------"
  )

  setupSimulation();
  setupWorld();
  simulationLoop();

  // time log
  if(benchmark::rolling::options.log)
    ru::timer->dumpToStdOuput();

  delete sim;
  return 0;
}

