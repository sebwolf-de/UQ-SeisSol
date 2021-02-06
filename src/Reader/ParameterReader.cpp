#include "ParameterReader.h"
#include "Eigen/src/Core/Matrix.h"
#include <cassert>
#include <yaml-cpp/emittermanip.h>
#include <iostream>

Reader::ParameterReader::ParameterReader(std::string filename) :
  root(YAML::LoadFile(filename)) {
    assert(("Parameter file contains link to SeisSol binary", root["SeisSolBinary"]));
    assert(("Parameter file contains number of receivers", root["NumberOfReceivers"]));
    assert(("Parameter file contains link to observation directory", root["ObservationDirectory"]));
  };

std::string Reader::ParameterReader::getSeisSolBinary() const {
  return root["SeisSolBinary"].as<std::string>();
}

std::string Reader::ParameterReader::getObservationDir() const {
  return root["ObservationDirectory"].as<std::string>();
}

size_t Reader::ParameterReader::getNumberOfReceivers() const {
  return root["NumberOfReceivers"].as<size_t>();
}

std::string Reader::ParameterReader::getReceiverPrefix() const {
  if (root["ReceiverPrefix"]) {
    return root["ReceiverPrefix"].as<std::string>();
  } else {
    return "observation";
  }
}

  
