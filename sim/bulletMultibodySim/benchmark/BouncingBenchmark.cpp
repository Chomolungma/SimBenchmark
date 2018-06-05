//
// Created by kangd on 15.02.18.
//

#include <BtMbSim.hpp>

#include "BouncingBenchmark.hpp"
#include "BtMbBenchmark.hpp"

bullet_mb_sim::BtMbSim *sim;
std::vector<bullet_mb_sim::ArticulatedSystemHandle> objList;
po::options_description desc;

void setupSimulation() {
  if (benchmark::bouncing::options.gui)
    sim = new bullet_mb_sim::BtMbSim(800, 600, 0.5, benchmark::NO_BACKGROUND);
  else
    sim = new bullet_mb_sim::BtMbSim();


  // erp
  if(benchmark::bouncing::options.erpYN)
    sim->setERP(benchmark::bouncing::params.erp, 0, 0);
  else
    sim->setERP(0, 0, 0);
  
  // timestep
  sim->setTimeStep(benchmark::bouncing::options.dt);

  // set up logger and timer
  if(benchmark::bouncing::options.log)
    benchmark::bouncing::loggerSetup(
        benchmark::bouncing::getLogDirpath(benchmark::bouncing::options.erpYN,
                                           benchmark::bouncing::options.e,
                                           benchmark::bulletmultibody::options.simName,
                                           benchmark::bulletmultibody::options.solverName,
                                           benchmark::bouncing::options.dt), "var"
    );
}

void resetWorld() {
  // materials
  // add objects
  auto checkerboard = sim->addArticulatedSystem(benchmark::bouncing::getBulletPlanepath(), bullet_mb_sim::object::URDF);
  checkerboard->setFrictionCoefficient(-1, benchmark::bouncing::params.mu_ground);
  checkerboard->setRestitutionCoefficient(-1, 1);

  for(int i = 0; i < benchmark::bouncing::params.n; i++) {
    for(int j = 0; j < benchmark::bouncing::params.n; j++) {
      auto ball = sim->addArticulatedSystem(benchmark::bouncing::getBulletBallPath(), bullet_mb_sim::object::URDF);
      ball->setGeneralizedCoordinate(
          {i * 2.0 - 10, 
           j * 2.0 - 10, 
           benchmark::bouncing::params.H,
           1, 0, 0, 0});
      ball->setFrictionCoefficient(-1, benchmark::bouncing::params.mu_ball);
      ball->setRestitutionCoefficient(-1, benchmark::bouncing::options.e);

      if(benchmark::bouncing::options.gui)
        ball.visual()[0]->setColor(
            {benchmark::bulletmultibody::color[0],
             benchmark::bulletmultibody::color[1],
             benchmark::bulletmultibody::color[2]});

      objList.push_back(ball);
    }
  }

  // gravity
  sim->setGravity({0, 0, benchmark::bouncing::params.g});

  if(benchmark::bouncing::options.gui) {
    sim->setLightPosition((float)benchmark::bouncing::params.lightPosition[0],
                          (float)benchmark::bouncing::params.lightPosition[1],
                          (float)benchmark::bouncing::params.lightPosition[2]);
//    sim->cameraFollowObject(checkerboard, {10, 0, 6});
  }
}

void simulationLoop() {
  if(benchmark::bouncing::options.gui) {
    // gui
    if(benchmark::bouncing::options.saveVideo)
      sim->startRecordingVideo("/tmp", "bullet-bouncing");

    for(int i = 0; i < (int) (benchmark::bouncing::params.T / benchmark::bouncing::options.dt)
        && sim->visualizerLoop(benchmark::bouncing::options.dt); i++) {
      sim->integrate();

      // energy log
      if(benchmark::bouncing::options.log) {
        double energy = 0;
        for(int j = 0; j < objList.size(); j++) {
          energy += objList[j]->getEnergy({0, 0, benchmark::bouncing::params.g});
        }
        rai::Utils::logger->appendData("energy", energy);
      }
    }

    if(benchmark::bouncing::options.saveVideo)
      sim->stopRecordingVideo();
  }
  else {
    // no gui
    if(benchmark::bouncing::options.log)
      ru::timer->startTimer("bouncing");

    for(int i = 0; i < (int) (benchmark::bouncing::params.T / benchmark::bouncing::options.dt); i++) {
      sim->integrate();

      if(benchmark::bouncing::options.log) {
        double energy = 0;
        for(int j = 0; j < objList.size(); j++) {
          energy += objList[j]->getEnergy({0, 0, benchmark::bouncing::params.g});
        }
        rai::Utils::logger->appendData("energy", energy);
      }
    }

    if(benchmark::bouncing::options.log)
      ru::timer->stopTimer("bouncing");
  }
}

int main(int argc, const char* argv[]) {

  benchmark::bouncing::addDescToOption(desc);
  benchmark::bulletmultibody::addDescToOption(desc);

  benchmark::bouncing::getOptionsFromArg(argc, argv, desc);
  benchmark::bulletmultibody::getOptionsFromArg(argc, argv, desc);

  benchmark::bouncing::getParamsFromYAML(benchmark::bouncing::getYamlPath().c_str(),
                                         benchmark::BULLET);

  RAIINFO(
      std::endl << "=======================" << std::endl
                << "Simulator: BULLET" << std::endl
                << "GUI      : " << benchmark::bouncing::options.gui << std::endl
                << "ERP      : " << benchmark::bouncing::options.erpYN << std::endl
                << "Res Coef : " << benchmark::bouncing::options.e << std::endl
                << "Timestep : " << benchmark::bouncing::options.dt << std::endl
                << "Solver   : " << benchmark::bulletmultibody::options.solverName << std::endl
                << "-----------------------"
  )

  setupSimulation();
  resetWorld();
  simulationLoop();

  // time log
//  if(benchmark::bouncing::options.log)
//    ru::timer->dumpToStdOuput();

  delete sim;
  return 0;
}