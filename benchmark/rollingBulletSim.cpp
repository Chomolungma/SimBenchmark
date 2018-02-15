//
// Created by kangd on 15.02.18.
//

#include <bulletSim/World_RG.hpp>

int main() {

  bullet_sim::World_RG bulletSim(800, 600, 0.5, bullet_sim::NO_BACKGROUND);
  bulletSim.setGravity({0, 0, -9.8});
  bulletSim.setLightPosition(30, 0, 10);

  // add objects
  auto checkerboard = bulletSim.addCheckerboard(5.0, 100.0, 100.0, 0.1);

  auto box1 = bulletSim.addBox(20, 20, 1, 1);
  box1->setPosition(0, 0, 0.5);
  box1->setVelocity(0, 10, 0, 0, 0, 0);
  box1->setExternalForce({0, 0, 0});

  auto ball1 = bulletSim.addSphere(0.5, 1);
  ball1->setPosition(0, -5, 1.5);
  auto ball2 = bulletSim.addSphere(0.5, 1);
  ball2->setPosition(0, 0, 1.5);
  auto ball3 = bulletSim.addSphere(0.5, 1);
  ball3->setPosition(0, 5, 1.5);

  // timestep
  double dt = 0.01;

  // camera relative position
  bulletSim.cameraFollowObject(checkerboard, {10, 0, 5});

  // simulation loop
  // press 'q' key to quit
  while (bulletSim.visualizerLoop(dt, 1)) {
    box1->setExternalForce({0, 32, 0});
    bulletSim.integrate(dt);
  }
  return 0;
}
