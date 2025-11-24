#pragma once
#include <filesystem>
#include <vector>
#include <string>

using namespace std;
namespace fs = std::filesystem;

class TestRunner {
public:
    TestRunner(const std::filesystem::path& configPath,
               const std::filesystem::path& resultsDir);

    void runAll();

private:
    std::vector<std::pair<int, int>> boardSizes;
    std::vector<std::string> testFiles;
    std::vector<int> lifeCellsPercentages;
    std::vector<int> threadCounts;
    std::vector<int> iterationsList;
    std::vector<std::string> algorithms;

    std::filesystem::path resultsDir;
    fs::path resultsFile;

    void loadConfig(const std::filesystem::path& path);

    double runTest(int width, int height, int threads,
                   int iterations, const std::string& algorithm,
                   const std::vector<std::pair<int, int>>& startingCells);

    void saveResult(int width, int height, int threads,
                    int iterations, const std::string& algorithm,
                    double time, const std::string& testType, int lifePercentage = -1,
                    const std::string& configFile = "");

    std::vector<std::pair<int, int>> loadTestFile(const fs::path& filePath, int& width, int& height);
    std::vector<std::pair<int, int>> generateRandomCells(int width, int height, int percentage, const std::string& configName = "");
};