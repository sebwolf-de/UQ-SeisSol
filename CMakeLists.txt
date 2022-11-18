cmake_minimum_required(VERSION 3.10)

project(UQ-SeisSol)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_library(io
  src/IO/ChainParameterWriter.cpp
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
target_link_libraries(seissol PUBLIC io)
target_link_libraries(main PUBLIC seissol)
target_link_libraries(main PUBLIC uq)

include_directories(${CMAKE_CURRENT_LIST_DIR}/src/)

find_package(Eigen3 REQUIRED)
target_include_directories(io SYSTEM PUBLIC ${EIGEN3_INCLUDE_DIR})
target_include_directories(uq SYSTEM PUBLIC ${EIGEN3_INCLUDE_DIR})
target_include_directories(seissol SYSTEM PUBLIC ${EIGEN3_INCLUDE_DIR})

find_package(MPI REQUIRED)
target_include_directories(io SYSTEM PUBLIC ${MPI_CXX_INCLUDE_PATH})
target_include_directories(main SYSTEM PUBLIC ${MPI_CXX_INCLUDE_PATH})
target_include_directories(seissol SYSTEM PUBLIC ${MPI_CXX_INCLUDE_PATH})
target_include_directories(uq SYSTEM PUBLIC ${MPI_CXX_INCLUDE_PATH})
target_link_libraries(main PUBLIC MPI::MPI_C)

find_package(HDF5 REQUIRED)
target_link_libraries(main PUBLIC ${HDF5_LIBRARIES} ${HDF5_LINK_LIBRARIES})
target_include_directories(main SYSTEM PUBLIC ${HDF5_INCLUDE_DIRS})

find_package(MUQ REQUIRED)
target_link_libraries(main PUBLIC ${MUQ_LIBRARIES} ${MUQ_LINK_LIBRARIES})
message(${MUQ_INCLUDE_DIRS})
target_include_directories(uq SYSTEM PUBLIC ${MUQ_INCLUDE_DIRS})

find_package(yaml-cpp REQUIRED)
target_link_libraries(io PUBLIC yaml-cpp)
target_link_libraries(uq PUBLIC yaml-cpp)

find_package(Boost COMPONENTS system filesystem graph)
target_include_directories(io SYSTEM PUBLIC ${Boost_INCLUDE_DIR})
find_package(spdlog CONFIG REQUIRED)
target_link_libraries(io PUBLIC spdlog::spdlog)
