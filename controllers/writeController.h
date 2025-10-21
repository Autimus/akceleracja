#pragma once

#include <string>
#include <vector>

void writeStartConfig(int columns, int rows, float simulationSpeed, const std::string& filename,const std::vector<std::pair<int,int>>& startingCells);

void saveStatistics(int threadsNumber, int cellsNumber, double time, const std::string& algorithmName, const std::string& filename);