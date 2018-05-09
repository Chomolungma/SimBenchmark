//
// Created by kangd on 09.05.18.
//

#include <DartWorld_RG.hpp>

#include "BuildingBenchmark.hpp"
#include "DartBenchmark.hpp"

dart_sim::DartWorld_RG *sim;
std::vector<benchmark::SingleBodyHandle> objectList;
po::options_description desc;

void setupSimulation() {
  if (benchmark::building::options.gui)
    sim = new dart_sim::DartWorld_RG(800, 600, 0.05,
                                     benchmark::NO_BACKGROUND,
                                     benchmark::dart::options.solverOption);
  else
    sim = new dart_sim::DartWorld_RG(benchmark::dart::options.solverOption);

  // time step
  sim->setTimeStep(benchmark::building::options.dt);

  /// no erp for dart
  if(benchmark::building::options.erpYN)
    RAIFATAL("erp is not supported for dart")

  // set up logger and timer
  if(benchmark::building::options.log)
    benchmark::building::loggerSetup(
        benchmark::building::getLogDirpath(benchmark::building::options.erpYN,
                                           benchmark::dart::options.simName,
                                           benchmark::dart::options.solverName,
                                           benchmark::building::options.dt), "var"
    );

  // increases max contact numbers
  sim->setMaxContacts(5000);
}

void setupWorld() {

  // add objects
  auto checkerboard = sim->addCheckerboard(10.0, 400.0, 400.0, 0.1, bo::BOX_SHAPE, 1, -1, bo::GRID);

  // block size
  const float shortLen = benchmark::building::params.shortLen;
  const float longLen = benchmark::building::params.longLen;
  const float heightLen = benchmark::building::params.heightLen;

  // num of blocks
  // numFloor x numBase + numFloor x (numWall x 2 + 1)
  const int numFloor = benchmark::building::params.numFloor;
  const int numBase = benchmark::building::params.numBase;
  const int numWall = numBase / 2;

  for(int i = 0; i < numFloor; i++) {
    // i floor
    for(int j = 0; j < numBase; j++) {
      // base
      auto base = sim->addBox(shortLen, longLen + 0.05, heightLen, 10.0);
      base.visual()[0]->setColor({0.0, 0.0, 1.0});
      base->setPosition(j * longLen, 0, i * heightLen * 2 + 0.05);
      objectList.push_back(base);
    }

    for(int j = 0; j < numWall; j++) {
      // right wall
      auto wall = sim->addBox(longLen, shortLen, heightLen, 10.0);
      wall.visual()[0]->setColor({0.0, 1.0, 0.0});
      wall->setPosition(j * longLen * 2 + 0.1, -0.5 * longLen, i * heightLen * 2 + 0.15);
      objectList.push_back(wall);
    }

    for(int j = 0; j < numWall - 1; j++) {
      // left wall
      auto wall = sim->addBox(longLen, shortLen, heightLen, 10.0);
      wall.visual()[0]->setColor({1.0, 0.0, 0.0});
      wall->setPosition(j * longLen * 2 + 0.3, 0.5 * longLen, i * heightLen * 2 + 0.15);
      objectList.push_back(wall);
    }

    // first wall on left
    auto wall1 = sim->addBox(longLen, shortLen, heightLen, 10.0);
    wall1.visual()[0]->setColor({1.0, 0.0, 0.0});
    wall1->setPosition(0.1, 0.5 * longLen, i * heightLen * 2 + 0.15);
    objectList.push_back(wall1);

    // last wall on left
    auto wall2 = sim->addBox(longLen, shortLen, heightLen, 10.0);
    wall2.visual()[0]->setColor({1.0, 0.0, 0.0});
    wall2->setPosition((numWall - 1) * longLen * 2 + 0.1, 0.5 * longLen, i * heightLen * 2 + 0.15);
    objectList.push_back(wall2);
  }

  if(benchmark::building::options.gui) {
    sim->setLightPosition((float)benchmark::building::params.lightPosition[0],
                          (float)benchmark::building::params.lightPosition[1],
                          (float)benchmark::building::params.lightPosition[2]);
    sim->cameraFollowObject(checkerboard, {0, 5, 2});
  }
}

void simulationLoop() {
  if(benchmark::building::options.gui) {
    // gui
    if(benchmark::building::options.saveVideo)
      sim->startRecordingVideo("/tmp", "dart-building");

    while(sim->visualizerLoop(benchmark::building::options.dt)) {

      if(objectList.back()->getPosition()[2] <
          benchmark::building::params.heightLen * (benchmark::building::params.numFloor - 1) * 2) {
        // break if the building collapses
        RAIINFO("building collapsed!")
        break;
      }

      sim->integrate();
    }

    if(benchmark::building::options.saveVideo)
      sim->stopRecordingVideo();
  }
  else {
    // no gui
    if(benchmark::building::options.log)
      ru::timer->startTimer("building");

    while(sim->visualizerLoop(benchmark::building::options.dt)) {

      if(objectList.back()->getPosition()[2] <
          benchmark::building::params.heightLen * (benchmark::building::params.numFloor - 1) * 2) {
        // break if the building collapses
        RAIINFO("building collapsed!")
        break;
      }

      sim->integrate();
    }

    if(benchmark::building::options.log)
      ru::timer->stopTimer("building");
  }
}

int main(int argc, const char* argv[]) {

  benchmark::building::addDescToOption(desc);
  benchmark::dart::addDescToOption(desc);

  benchmark::building::getOptionsFromArg(argc, argv, desc);
  benchmark::dart::getOptionsFromArg(argc, argv, desc);

  benchmark::building::getParamsFromYAML(benchmark::building::getYamlpath().c_str(),
                                         benchmark::DART);

  RAIINFO(
      std::endl << "=======================" << std::endl
                << "Simulator: DART" << std::endl
                << "GUI      : " << benchmark::building::options.gui << std::endl
                << "ERP      : " << benchmark::building::options.erpYN << std::endl
                << "Timestep : " << benchmark::building::options.dt << std::endl
                << "Num block: " << objectList.size() << std::endl
                << "Solver   : " << benchmark::dart::options.solverName << std::endl
                << "-----------------------"
  )

  setupSimulation();
  setupWorld();
  simulationLoop();

  // time log
  if(benchmark::building::options.log)
    ru::timer->dumpToStdOuput();

  delete sim;
  return 0;
}
