#include <fstream>
#include "writeController.h"

void writeStartConfig(int columns, int rows, float simulationSpeed, std::filesystem::path dir, std::string filename, std::vector<std::pair<int,int>>& startingCells) {
    for (auto& c : filename)
        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    std::ofstream file(dir/filename, std::ios::out);
    file << columns << "\n" << rows << "\n" << simulationSpeed << "\n";
    for (auto [first, second] : startingCells) {
        file << first << " " << second << "\n";
    }
    file.close();
}

void saveStatistics(int threadsNumber, int cellsNumber, double time,std::string& algorithmName, std::filesystem::path dir) {
    std::ofstream file;
    std::filesystem::path filename = dir/algorithmName.append(".txt");
    if (!std::filesystem::exists(filename)) {
        file = std::ofstream(filename, std::ios::out);
        file << "algorithmName\tthreadsNumber\tcellsNumber\ttime[s]\n";
        file.close();
    }
    file = std::ofstream(filename, std::ios::app);
    file << algorithmName << "\t" << threadsNumber << "\t" <<cellsNumber << "\t" << time << "\n";
    file.close();
}