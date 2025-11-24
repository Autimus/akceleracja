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
    std::vector<int> threadCounts;
    std::vector<int> iterationsList;
    std::vector<std::string> algorithms;

    std::filesystem::path resultsDir;
    fs::path resultsFile;

    void loadConfig(const std::filesystem::path& path);

    double runTest(int width, int height, int threads,
        int iterations, const std::string& algorithm);

    void saveResult(int width, int height, int threads,
        int iterations, const std::string& algorithm,
        double time);
};
