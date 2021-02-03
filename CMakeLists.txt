cmake_minimum_required(VERSION 3.10)

project(UQ-SeisSol)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_library(uq 
  src/UQ/SamplingProblem.cpp
  src/UQ/MIInterpolation.cpp
  src/UQ/MIComponentFactory.cpp)
add_library(seissol
  src/SeisSol/Runner.cpp)

add_executable(main src/main.cpp)
target_link_libraries(main uq)
target_link_libraries(main seissol)

include_directories(${CMAKE_CURRENT_LIST_DIR}/src/)

find_package(Eigen3 REQUIRED)

find_package(MUQ REQUIRED)
target_link_libraries(main ${MUQ_LIBRARIES} ${MUQ_LINK_LIBRARIES})
include_directories(${MUQ_INCLUDE_DIRS})
