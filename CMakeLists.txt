cmake_minimum_required(VERSION 3.10)

project(UQ-SeisSol)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

#Use clang tidy for static code analysis
set(CMAKE_CXX_CLANG_TIDY "clang-tidy-9")

add_library(io
  src/IO/MaterialParameterWriter.cpp
  src/IO/ParameterReader.cpp
  src/IO/ReceiverReader.cpp)
  
add_library(seissol
  src/SeisSol/UQSeisSolFactory.cpp
  src/SeisSol/Receiver.cpp
  src/SeisSol/ReceiverDB.cpp
  src/SeisSol/Runner.cpp)

add_library(uq 
  src/UQ/MIComponentFactory.cpp
  src/UQ/MIInterpolation.cpp
  src/UQ/SamplingProblem.cpp)

add_executable(main src/main.cpp)
target_link_libraries(seissol io)
target_link_libraries(main io)
target_link_libraries(main seissol)
target_link_libraries(main uq)

include_directories(${CMAKE_CURRENT_LIST_DIR}/src/)

find_package(Eigen3 REQUIRED)

find_package(MUQ REQUIRED)
target_link_libraries(main ${MUQ_LIBRARIES} ${MUQ_LINK_LIBRARIES})
include_directories(SYSTEM ${MUQ_INCLUDE_DIRS})

find_package(yaml-cpp REQUIRED)
target_link_libraries(main ${YAML_CPP_LIBRARIES})
include_directories(SYSTEM ${YAML_CPP_INCLUDE_DIR})

find_package(Boost COMPONENTS system filesystem graph)

