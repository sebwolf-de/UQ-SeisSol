cmake_minimum_required(VERSION 3.10)

project(UQ-SeisSol)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_library(reader
  src/Reader/ParameterReader.cpp
  src/Reader/ReceiverReader.cpp)
  
add_library(seissol
  src/SeisSol/Receiver.cpp
  src/SeisSol/ReceiverList.cpp
  src/SeisSol/Runner.cpp)

add_library(uq 
  src/UQ/MIComponentFactory.cpp
  src/UQ/MIInterpolation.cpp
  src/UQ/SamplingProblem.cpp)

add_executable(main src/main.cpp)
target_link_libraries(seissol reader)
target_link_libraries(main reader)
target_link_libraries(main seissol)
target_link_libraries(main uq)

include_directories(${CMAKE_CURRENT_LIST_DIR}/src/)

find_package(Eigen3 REQUIRED)

find_package(MUQ REQUIRED)
target_link_libraries(main ${MUQ_LIBRARIES} ${MUQ_LINK_LIBRARIES})
include_directories(${MUQ_INCLUDE_DIRS})

find_package(yaml-cpp REQUIRED)
target_link_libraries(main ${YAML_CPP_LIBRARIES})
include_directories(${YAML_CPP_INCLUDE_DIR})

