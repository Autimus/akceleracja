cmake_minimum_required(VERSION 3.21)
project(akceleracja LANGUAGES CXX)

### USTAWIENIA OPCJI
option(USE_CUDA "Enable CUDA support" ON)
option(USE_OPENCL "Enable OpenCL support" ON)
###

set(CMAKE_CXX_STANDARD 20)

if(USE_CUDA)
    enable_language(CUDA)
    set(CMAKE_CUDA_STANDARD 17)
    set(CMAKE_CUDA_ARCHITECTURES 86)
endif()

set(SOURCES
    main.cpp
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
      testing/TestRunner.cpp
    testing/TestRunner.h
)

# Pliki CUDA s� dodawane tylko je�li CUDA jest w��czone
if(USE_CUDA)
    list(APPEND SOURCES
        algorithms/gpuBasic.cu
        algorithms/gpuBasic.h
        algorithms/gpuOpt1.h
        algorithms/gpuOpt1.cu
        algorithms/gpuOpt2.h
        algorithms/gpuOpt2.cu
    )
endif()

add_executable(akceleracja ${SOURCES})

if(USE_CUDA)
    set_target_properties(akceleracja PROPERTIES
        CUDA_SEPARABLE_COMPILATION ON
        CUDA_ARCHITECTURES 86
    )
endif()

if(USE_OPENCL)
    find_package(OpenCL REQUIRED)
    target_link_libraries(akceleracja PRIVATE OpenCL::OpenCL)
endif()
