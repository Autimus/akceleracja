#include "TestRunner.h"
#include <fstream>
#include <iostream>
#include <chrono>
#include <iomanip>

#include "../gameLogic/GameInstance.h"
#include "../algorithms/cpuLinear.h"
#include "../algorithms/cpuParallel.h"
#ifdef USE_CUDA
#include "../algorithms/gpuBasic.h"
#include "../algorithms/gpuOpt2.h"
#endif
#include "../controllers/writeController.h"

// Konstruktor - inicjalizacja pliku wynikowego i odpowiednich zmiennych
TestRunner::TestRunner(const fs::path& configPath,
    const fs::path& resultsDir)
    : resultsDir(resultsDir)
{
    loadConfig(configPath);

    auto now = chrono::system_clock::now();
    time_t t_now = chrono::system_clock::to_time_t(now);
    tm local_tm;
#ifdef _WIN32
    localtime_s(&local_tm, &t_now);
#else
    localtime_r(&t_now, &local_tm);
#endif
    char timeStr[20];
    strftime(timeStr, sizeof(timeStr), "%Y%m%d_%H%M%S", &local_tm);

    resultsFile = resultsDir / ("results_" + string(timeStr) + ".csv"); // np. results/results_20251124_153210.csv

    fs::create_directories(resultsDir);

    ofstream out(resultsFile);
    out << "timestamp;algorithm;width;height;threads;iterations;time_s\n";
}

// £adowanie konfiguracji z pliku 
void TestRunner::loadConfig(const fs::path& path) {
    ifstream file(path);
    if (!file.is_open()) {
        cout << path << endl;
        throw runtime_error("Nie mogê otworzyæ pliku testowego.");
    }

    string line;
    enum Section { NONE, SIZES, THREADS, ITER, ALG };
    Section current = NONE;

    while (getline(file, line)) {
        if (line.empty()) continue;

        if (line == "BOARD_SIZE(WIDTHxHEIGHT)") { current = SIZES; continue; }
        if (line == "THREADS_NUMBER") { current = THREADS; continue; }
        if (line == "ITERATIONS") { current = ITER; continue; }
        if (line == "ALGORITHMS") { current = ALG; continue; }

        switch (current) {
        case SIZES: {
            int w, h;
            if (sscanf(line.c_str(), "%dx%d", &w, &h) == 2)
                boardSizes.push_back({ w, h });
            break;
        }
        case THREADS: {
            threadCounts.push_back(stoi(line));
            break;
        }
        case ITER: {
            iterationsList.push_back(stoi(line));
            break;
        }
        case ALG: {
            algorithms.push_back(line);
            break;
        }
        default:
            break;
        }
    }

    if (threadCounts.empty()) threadCounts.push_back(1);
    if (iterationsList.empty()) iterationsList.push_back(200);
}


// Pojedynczy test
double TestRunner::runTest(int width, int height, int threads,
    int iterations, const string& algorithm)
{
    vector<pair<int, int>> empty;
    GameInstance game(width, height, empty);

    if (algorithm == "cpulinear") {
        return cpuLinear(game, iterations);
    }

    if (algorithm == "cpuparallel") {
        return cpuParallel(game, threads, iterations);
    }

#ifdef USE_CUDA
    if (algorithm == "gpu1") {
        return gpuBasic(game, iterations);
    }

    if (algorithm == "gpu2") {
        return gpuOpt2(game, iterations);
    }
#else
    if (algorithm == "gpu1" || algorithm == "gpu2") {
        cout << "[INFO] GPU pominiête — CUDA OFF\n";
        return -1.0;
    }
#endif

    cerr << "Nieznany algorytm: " << algorithm << endl;
    return -1.0;
}

// Zapisywanie pojedynczego wyniku do pliku wynikowego
void TestRunner::saveResult(int width, int height, int threads,
    int iterations, const string& algorithm,
    double time)
{
    auto now = chrono::system_clock::now();
    time_t t_now = chrono::system_clock::to_time_t(now);
    tm local_tm;
#ifdef _WIN32
    localtime_s(&local_tm, &t_now);
#else
    localtime_r(&t_now, &local_tm);
#endif
    char timeStr[20];
    strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", &local_tm);

    ofstream out(resultsFile, ios::app);
    out << timeStr << ";" << algorithm << ";" << width << ";" << height << ";"
        << threads << ";" << iterations << ";" << time << "\n";
    out.flush();
}

// Wszystkie testy
void TestRunner::runAll() {
    for (auto& size : boardSizes) {
        for (auto iter : iterationsList) {
            for (auto threads : threadCounts) {
                for (auto& alg : algorithms) {

                    cout << "--- Test: "
                        << size.first << "x" << size.second
                        << ", threads=" << threads
                        << ", iterations=" << iter
                        << ", alg=" << alg
                        << " ---\n";

                    double t = runTest(size.first, size.second,
                        threads, iter, alg);

                    saveResult(size.first, size.second,
                        threads, iter, alg, t);
                }
            }
        }
    }

    cout << "[WYNIKI] Plik: " << resultsFile << endl;
}
