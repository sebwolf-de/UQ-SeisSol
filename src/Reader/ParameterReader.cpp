#include "ParameterReader.h"
#include "Eigen/src/Core/Matrix.h"
#include <cassert>
#include <yaml-cpp/emittermanip.h>
#include <iostream>

Reader::ParameterReader::ParameterReader(std::string filename) :
  root(YAML::LoadFile(filename)) {
    assert(("Parameter file contains link to SeisSol binary", root["SeisSol"]));
    assert(("Parameter file contains number of receivers", root["ReceiverNumber"]));
    assert(("Parameter file contains link to observation directory", root["ObservationDir"]));
  };

std::string Reader::ParameterReader::getSeisSolBinary() const {
  return root["SeisSol"].as<std::string>();
}

std::string Reader::ParameterReader::getObservationDir() const {
  return root["ObservationDir"].as<std::string>();
}

size_t Reader::ParameterReader::getReceiverNumber() const {
  return root["ReceiverNumber"].as<size_t>();
}

std::string Reader::ParameterReader::getPrefix() const {
  if (root["Prefix"]) {
    return root["Prefix"].as<std::string>();
  } else {
    return "observation";
  }
}

  
