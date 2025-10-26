#pragma once

#include <string>
#include <vector>
#include <filesystem>

void writeStartConfig(int columns, int rows, float simulationSpeed, std::filesystem::path dir, std::string filename, std::vector<std::pair<int,int>>& startingCells);

void saveStatistics(int threadsNumber, int cellsNumber, double time,std::string& algorithmName, std::filesystem::path dir);