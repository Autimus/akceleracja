cmake_minimum_required(VERSION 3.21)
project(akceleracja LANGUAGES CXX CUDA)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CUDA_STANDARD 17)

set(CMAKE_CUDA_ARCHITECTURES 86)

add_executable(akceleracja main.cpp
        tools/switchCase.h
        tools/Timer.h
        controllers/readStartConfig.h
        controllers/readStartConfig.cpp
        controllers/writeController.cpp
        controllers/writeController.h
        tools/EnvVar.cpp
        tools/EnvVar.h
        tools/countGPUThreads.h
        algorithms/cpuLinear.cpp
        algorithms/cpuLinear.h
        controllers/VisualizationController.cpp
        controllers/VisualizationController.h
        tools/BetterRandom.h
        gameLogic/GameInstance.cpp
        gameLogic/GameInstance.h
        tools/clearScreen.h
        algorithms/cpuParallel.cpp
        algorithms/cpuParallel.h
        algorithms/gpuBasic.cu
        algorithms/gpuBasic.h
        algorithms/gpuOpt1.h
        algorithms/gpuOpt1.cu
        algorithms/gpuOpt2.h
        algorithms/gpuOpt2.cu

)
set_target_properties(akceleracja PROPERTIES
        CUDA_SEPARABLE_COMPILATION ON
        CUDA_ARCHITECTURES 86
        )
find_package(OpenCL REQUIRED)
target_link_libraries(akceleracja PRIVATE OpenCL::OpenCL)
