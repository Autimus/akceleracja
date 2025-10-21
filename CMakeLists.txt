cmake_minimum_required(VERSION 4.0)
project(akceleracja)

set(CMAKE_CXX_STANDARD 20)

add_executable(akceleracja main.cpp
        tools/switchCase.h
        tools/Timer.h
        controllers/readStartConfig.h
        controllers/readStartConfig.cpp
        controllers/writeController.cpp
        controllers/writeController.h
        tools/EnvVar.cpp
        tools/EnvVar.h
)
