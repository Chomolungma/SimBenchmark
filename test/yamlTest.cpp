//
// Created by kangd on 14.03.18.
//

#include <iostream>
#include <yaml-cpp/yaml.h>

int main() {

  YAML::Node config = YAML::LoadFile("./benchmark/rolling.yaml");

  std::cout << config["light_position"] << std::endl;

  std::vector<double> vector = config["light_position"].as<std::vector<double>>();

  return 0;
}
