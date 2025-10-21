#pragma once

#include <chrono>

// Timer wzięty z https://www.geeksforgeeks.org/chrono-in-c/
class Timer {
private:
    std::chrono::time_point<std::chrono::system_clock> start;

public:
    inline void Start() {
        start = std::chrono::system_clock::now();
    }

    inline double Stop() {
        std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_seconds = end - start;
        return elapsed_seconds.count();
    }
};