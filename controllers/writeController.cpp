#include <fstream>
#include <filesystem>
#include "writeController.h"

void writeStartConfig(const int columns, const int rows, const float simulationSpeed,  const std::string& filename, const std::vector<std::pair<int,int>>& startingCells) {
    std::ofstream file(filename, std::ios::out);
    file << columns << "\n" << rows << "\n" << simulationSpeed << "\n";
    for (auto [first, second] : startingCells) {
        file << first << " " << second << "\n";
    }
    file.close();
}

void saveStatistics(int threadsNumber, int cellsNumber, double time, const std::string& algorithmName, const std::string& filename) {
    std::ofstream file;
    if (!std::filesystem::exists(filename)) {
        file = std::ofstream(filename, std::ios::out);
        file << "algorithmName\tthreadsNumber\tcellsNumber\ttime[s]\n";
        file.close();
    }
    file = std::ofstream(filename, std::ios::app);
    file << algorithmName << "\t" << threadsNumber << "\t" <<cellsNumber << "\t" << time << "\n";
    file.close();
}