#include "TestRunner.h"
#include <fstream>
#include <iostream>
#include <chrono>
#include <iomanip>
#include <random>
#include <sstream>
#include <algorithm>
#include <set>

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

    resultsFile = resultsDir / ("results_" + string(timeStr) + ".csv");

    fs::create_directories(resultsDir);

    ofstream out(resultsFile);
    out << "timestamp;algorithm;width;height;threads;iterations;time_s;test_type;life_percentage;config_file\n";
    out.close();
}

// Ładowanie konfiguracji z pliku
void TestRunner::loadConfig(const fs::path& path) {
    ifstream file(path);
    if (!file.is_open()) {
        cout << "Nie mogę otworzyć pliku konfiguracyjnego: " << path << endl;
        throw runtime_error("Nie mogę otworzyć pliku testowego.");
    }

    string line;
    enum Section { NONE, SIZES, TEST_FILES, LIFE_PERCENTAGE, THREADS, ITER, ALG };
    Section current = NONE;

    while (getline(file, line)) {
        // Usuń białe znaki z początku i końca
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1);

        if (line.empty()) continue;

        if (line == "BOARD_SIZE(WIDTHxHEIGHT)") { current = SIZES; continue; }
        if (line == "TEST_FILES") { current = TEST_FILES; continue; }
        if (line == "LIFE_CELLS_PERCENTAGE") { current = LIFE_PERCENTAGE; continue; }
        if (line == "THREADS_NUMBER") { current = THREADS; continue; }
        if (line == "ITERATIONS") { current = ITER; continue; }
        if (line == "ALGORITHMS") { current = ALG; continue; }

        switch (current) {
            case SIZES: {
                int w, h;
                if (sscanf(line.c_str(), "%dx%d", &w, &h) == 2) {
                    if (w > 0 && h > 0) {
                        boardSizes.push_back({ w, h });
                    }
                }
                break;
            }
            case TEST_FILES: {
                if (!line.empty()) {
                    // Używamy ścieżek bezwzględnych
                    testFiles.push_back(line);
                }
                break;
            }
            case LIFE_PERCENTAGE: {
                try {
                    int percentage = stoi(line);
                    if (percentage > 0 && percentage <= 100) {
                        lifeCellsPercentages.push_back(percentage);
                    }
                }
                catch (const exception& e) {
                    cerr << "Błąd parsowania procentu: " << line << endl;
                }
                break;
            }
            case THREADS: {
                try {
                    int threads = stoi(line);
                    if (threads > 0) {
                        threadCounts.push_back(threads);
                    }
                }
                catch (const exception& e) {
                    cerr << "Błąd parsowania liczby wątków: " << line << endl;
                }
                break;
            }
            case ITER: {
                try {
                    int iter = stoi(line);
                    if (iter > 0) {
                        iterationsList.push_back(iter);
                    }
                }
                catch (const exception& e) {
                    cerr << "Błąd parsowania iteracji: " << line << endl;
                }
                break;
            }
            case ALG: {
                if (!line.empty()) {
                    algorithms.push_back(line);
                }
                break;
            }
            default:
                break;
        }
    }

    // Ustaw wartości domyślne jeśli sekcje są puste
    if (boardSizes.empty()) {
        boardSizes.push_back({ 10, 10 });
        cout << "[INFO] Używam domyślnego rozmiaru planszy: 10x10\n";
    }
    if (threadCounts.empty()) {
        threadCounts.push_back(1);
        cout << "[INFO] Używam domyślnej liczby wątków: 1\n";
    }
    if (iterationsList.empty()) {
        iterationsList.push_back(100);
        cout << "[INFO] Używam domyślnej liczby iteracji: 100\n";
    }
    if (lifeCellsPercentages.empty()) {
        lifeCellsPercentages.push_back(30);
        cout << "[INFO] Używam domyślnego procentu komórek: 30%\n";
    }
    if (algorithms.empty()) {
        algorithms.push_back("cpulinear");
        cout << "[INFO] Używam domyślnego algorytmu: cpulinear\n";
    }

    file.close();
}

// Wczytuje plik testowy i zwraca komórki startowe z automatycznym rozszerzeniem planszy
std::vector<std::pair<int, int>> TestRunner::loadTestFile(const fs::path& filePath, int& width, int& height) {
    std::vector<std::pair<int, int>> startingCells;

    ifstream file(filePath);

    if (!file.is_open()) {
        cerr << "Nie mogę otworzyć pliku testowego: " << filePath << endl;
        return startingCells;
    }

    string line;
    width = 0;
    height = 0;

    // Wczytaj wszystkie komórki i znajdź maksymalne współrzędne
    while (getline(file, line)) {
        if (line.empty()) continue;

        istringstream iss(line);
        int x, y;
        if (iss >> x >> y) {
            startingCells.emplace_back(x, y);
            width = max(width, x + 1);  // Rozszerz szerokość jeśli potrzeba
            height = max(height, y + 1); // Rozszerz wysokość jeśli potrzeba
        }
    }

    file.close();

    // Jeśli nie znaleziono żadnych komórek lub rozmiar jest za mały
    if (width <= 0) width = 10;
    if (height <= 0) height = 10;

    cout << "Wczytano plik: " << filePath << " - automatyczny rozmiar: " << width << "x" << height
         << ", komórek: " << startingCells.size() << endl;

    return startingCells;
}

// Generuje losowe komórki na podstawie procentu i zapisuje do pliku
std::vector<std::pair<int, int>> TestRunner::generateRandomCells(int width, int height, int percentage, const std::string& configName) {
    std::vector<std::pair<int, int>> cells;

    if (percentage <= 0 || percentage > 100 || width <= 0 || height <= 0) {
        cerr << "Nieprawidłowe parametry do generowania komórek: "
             << width << "x" << height << ", " << percentage << "%" << endl;
        return cells;
    }

    int totalCells = width * height;
    int cellsToGenerate = max(1, totalCells * percentage / 100);

    cout << "Generowanie " << cellsToGenerate << " losowych komórek ("
         << percentage << "%) dla planszy " << width << "x" << height << endl;

    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> distX(0, width - 1);
    uniform_int_distribution<> distY(0, height - 1);

    set<pair<int, int>> uniqueCells;

    while (uniqueCells.size() < cellsToGenerate) {
        int x = distX(gen);
        int y = distY(gen);
        uniqueCells.insert({ x, y });
    }

    cells.assign(uniqueCells.begin(), uniqueCells.end());

    // Zapisz wygenerowaną konfigurację do pliku
    fs::path configsDir = resultsDir / "generated_configs";
    fs::create_directories(configsDir);

    string filename = "random_" + to_string(width) + "x" + to_string(height) +
                      "_" + to_string(percentage) + "pct_" + configName + ".txt";
    fs::path filePath = configsDir / filename;

    ofstream configFile(filePath);
    if (configFile.is_open()) {
        // Zapisz wszystkie komórki
        for (const auto& cell : cells) {
            configFile << cell.first << " " << cell.second << "\n";
        }
        configFile.close();
        cout << "Zapisano konfigurację do: " << filePath << endl;
    }
    else {
        cerr << "Nie udało się zapisać konfiguracji: " << filePath << endl;
    }

    return cells;
}

// Pojedynczy test
double TestRunner::runTest(int width, int height, int threads,
                           int iterations, const string& algorithm,
                           const vector<pair<int, int>>& startingCells)
{
    try {
        if (width <= 0 || height <= 0) {
            cerr << "Nieprawidłowy rozmiar planszy: " << width << "x" << height << endl;
            return -1.0;
        }

        if (iterations <= 0) {
            cerr << "Nieprawidłowa liczba iteracji: " << iterations << endl;
            return -1.0;
        }

        cout << "Uruchamianie testu: " << width << "x" << height
             << ", threads=" << threads << ", iterations=" << iterations
             << ", alg=" << algorithm << ", komórek startowych: " << startingCells.size() << endl;

        vector<pair<int, int>> cellsCopy = startingCells;
        GameInstance game(width, height, cellsCopy);

        double result = -1.0;

        if (algorithm == "cpulinear") {
            result = cpuLinear(game, iterations);
        }
        else if (algorithm == "cpuparallel") {
            result = cpuParallel(game, threads, iterations);
        }
#ifdef USE_CUDA
        else if (algorithm == "gpu1") {
            result = gpuBasic(game, iterations);
        }
        else if (algorithm == "gpu2") {
            result = gpuOpt2(game, iterations);
        }
#else
            else if (algorithm == "gpu1" || algorithm == "gpu2") {
            cout << "[INFO] GPU pominięte — CUDA OFF\n";
            return -1.0;
        }
#endif
        else {
            cerr << "Nieznany algorytm: " << algorithm << endl;
            return -1.0;
        }

        cout << "Test zakończony, czas: " << result << "s" << endl;
        return result;

    }
    catch (const exception& e) {
        cerr << "Błąd podczas wykonywania testu: " << e.what() << endl;
        return -1.0;
    }
    catch (...) {
        cerr << "Nieznany błąd podczas wykonywania testu" << endl;
        return -1.0;
    }
}

// Zapisywanie pojedynczego wyniku do pliku wynikowego
void TestRunner::saveResult(int width, int height, int threads,
                            int iterations, const string& algorithm,
                            double time, const string& testType, int lifePercentage, const string& configFile)
{
    try {
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
        if (!out.is_open()) {
            cerr << "Nie mogę otworzyć pliku wynikowego do zapisu: " << resultsFile << endl;
            return;
        }

        out << timeStr << ";" << algorithm << ";" << width << ";" << height << ";"
            << threads << ";" << iterations << ";" << time << ";" << testType;

        if (lifePercentage != -1) {
            out << ";" << lifePercentage;
        }
        else {
            out << ";";
        }

        out << ";" << configFile << "\n";
        out.close();

    }
    catch (const exception& e) {
        cerr << "Błąd zapisu wyniku: " << e.what() << endl;
    }
}

// Wszystkie testy
// Wszystkie testy
void TestRunner::runAll() {
    cout << "=== URUCHAMIANIE WSZYSTKICH TESTÓW ===" << endl;

    // Testy z plikami konfiguracyjnymi (ścieżki bezwzględne)
    if (!testFiles.empty()) {
        cout << "\n=== TESTY Z PLIKAMI KONFIGURACYJNYMI ===" << endl;

        for (const auto& testFile : testFiles) {
            fs::path filePath(testFile); // Używamy ścieżki bezwzględnej

            if (!fs::exists(filePath)) {
                cerr << "Plik nie istnieje: " << filePath << endl;
                continue;
            }

            int fileWidth = 0, fileHeight = 0;
            auto startingCells = loadTestFile(filePath, fileWidth, fileHeight);

            if (startingCells.empty()) {
                cerr << "Pominięto plik (błąd wczytywania lub pusty): " << testFile << endl;
                continue;
            }

            cout << "\n--- Test z pliku: " << testFile
                 << " (automatyczny rozmiar: " << fileWidth << "x" << fileHeight
                 << ", " << startingCells.size() << " komórek) ---" << endl;

            for (auto iter : iterationsList) {
                for (auto threads : threadCounts) {
                    for (auto& alg : algorithms) {
                        double t;
                        int actualThreads = threads;

                        if (alg == "gpu1" || alg == "gpu2") {
                            // Oblicz liczbę wątków GPU
                            int blocksX = (fileWidth + 16 - 1) / 16;
                            int blocksY = (fileHeight + 16 - 1) / 16;
                            int totalBlocks = blocksX * blocksY;
                            int threadsPerBlock = 16 * 16; // 256
                            actualThreads = totalBlocks * threadsPerBlock;

                            cout << "Plik: " << filePath.filename().string()
                                 << ", GPU_threads=" << actualThreads
                                 << ", iterations=" << iter
                                 << ", alg=" << alg << endl;

                            t = runTest(fileWidth, fileHeight, actualThreads, iter, alg, startingCells);
                        }
                        else if (alg == "cpuparallel") {
                            cout << "Plik: " << filePath.filename().string()
                                 << ", CPU_threads=" << threads
                                 << ", iterations=" << iter
                                 << ", alg=" << alg << endl;

                            t = runTest(fileWidth, fileHeight, threads, iter, alg, startingCells);
                        }
                        else if (alg == "cpulinear") {
                            actualThreads = 1;
                            cout << "Plik: " << filePath.filename().string()
                                 << ", CPU_threads=" << actualThreads
                                 << ", iterations=" << iter
                                 << ", alg=" << alg << endl;

                            t = runTest(fileWidth, fileHeight, actualThreads, iter, alg, startingCells);
                        }
                        else {
                            // Dla innych algorytmów używamy podanej liczby wątków
                            t = runTest(fileWidth, fileHeight, threads, iter, alg, startingCells);
                        }

                        if (t >= 0) {
                            saveResult(fileWidth, fileHeight, actualThreads, iter, alg, t, "file_test", -1, filePath.filename().string());
                            cout << "Zapisano wynik: " << t << "s" << endl;
                        }
                        else {
                            cout << "Test zakończony błędem" << endl;
                        }
                    }
                }
            }
        }
    }

    // Testy z losowym zapełnieniem
    if (!boardSizes.empty() && !lifeCellsPercentages.empty()) {
        cout << "\n=== TESTY Z LOSOWYM ZAPEŁNIENIEM ===" << endl;

        for (auto& size : boardSizes) {
            for (auto percentage : lifeCellsPercentages) {
                // Generuj unikalną nazwę konfiguracji
                string configName = to_string(size.first) + "x" + to_string(size.second) + "_" + to_string(percentage);

                auto startingCells = generateRandomCells(size.first, size.second, percentage, configName);

                if (startingCells.empty()) {
                    cerr << "Nie udało się wygenerować komórek dla: "
                         << size.first << "x" << size.second << ", " << percentage << "%" << endl;
                    continue;
                }

                cout << "\n--- Losowy test: " << size.first << "x" << size.second
                     << ", " << percentage << "% komórek ("
                     << startingCells.size() << " komórek) ---" << endl;

                for (auto iter : iterationsList) {
                    for (auto threads : threadCounts) {
                        for (auto& alg : algorithms) {
                            double t;
                            int actualThreads = threads;

                            if (alg == "gpu1" || alg == "gpu2") {
                                // Oblicz liczbę wątków GPU
                                int blocksX = (size.first + 16 - 1) / 16;
                                int blocksY = (size.second + 16 - 1) / 16;
                                int totalBlocks = blocksX * blocksY;
                                int threadsPerBlock = 16 * 16; // 256
                                actualThreads = totalBlocks * threadsPerBlock;

                                cout << "Losowy: " << size.first << "x" << size.second
                                     << ", " << percentage << "%, GPU_threads=" << actualThreads
                                     << ", iterations=" << iter
                                     << ", alg=" << alg << endl;

                                t = runTest(size.first, size.second, actualThreads, iter, alg, startingCells);
                            }
                            else if (alg == "cpuparallel") {
                                cout << "Losowy: " << size.first << "x" << size.second
                                     << ", " << percentage << "%, CPU_threads=" << threads
                                     << ", iterations=" << iter
                                     << ", alg=" << alg << endl;

                                t = runTest(size.first, size.second, threads, iter, alg, startingCells);
                            }
                            else if (alg == "cpulinear") {
                                actualThreads = 1;
                                cout << "Losowy: " << size.first << "x" << size.second
                                     << ", " << percentage << "%, CPU_threads=" << actualThreads
                                     << ", iterations=" << iter
                                     << ", alg=" << alg << endl;

                                t = runTest(size.first, size.second, actualThreads, iter, alg, startingCells);
                            }
                            else {
                                // Dla innych algorytmów używamy podanej liczby wątków
                                t = runTest(size.first, size.second, threads, iter, alg, startingCells);
                            }

                            if (t >= 0) {
                                string configFilename = "random_" + to_string(size.first) + "x" +
                                                        to_string(size.second) + "_" + to_string(percentage) +
                                                        "pct_" + configName + ".txt";
                                saveResult(size.first, size.second, actualThreads, iter, alg, t, "random_test", percentage, configFilename);
                                cout << "Zapisano wynik: " << t << "s" << endl;
                            }
                            else {
                                cout << "Test zakończony błędem" << endl;
                            }
                        }
                    }
                }
            }
        }
    }

    cout << "\n=== TESTY ZAKOŃCZONE ===" << endl;
    cout << "[WYNIKI] Plik: " << resultsFile << endl;
}