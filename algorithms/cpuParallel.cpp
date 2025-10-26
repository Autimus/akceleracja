#include <vector>
#include <thread>
#include <atomic>
#include <barrier>
#include <iostream>
#include "../gameLogic/GameInstance.h"
#include "../tools/Timer.h"
using namespace std;

void cpuParallel(GameInstance& game, int threads) {
    int columns = game.getColumns();
    int rows = game.getRows();

    // Podział wierszy między wątki
    int rowsPerThread = rows / threads;
    int remainder = rows % threads;

    vector<thread> workers;

    int startRow = 0;
    for (int t = 0; t < threads; ++t) {
        int endRow = startRow + rowsPerThread + (t < remainder ? 1 : 0);

        // Tworzy i uruchamia wątek, który przetwarza swój zakres rzędów
        workers.emplace_back([&game, startRow, endRow, columns]() {
            for (int y = startRow; y < endRow; ++y) {
                for (int x = 0; x < columns; ++x) {
                    game.processCell(x, y);
                }
            }
        });

        startRow = endRow;
    }

    // Synchronizacja wątków — czekanie na zakończenie każdego z nich
    for (auto& w : workers) {
        w.join();
    }

    // Zastosowanie zmian w gameArea
    game.applyIteration();
}

void threadFunc(GameInstance &game, int startRow, int endRow, int columns, std::barrier<> &sync_point, int iterations) {
    for (int i = 0; i < iterations; ++i) {
        for (int y = startRow; y < endRow; ++y)
            for (int x = 0; x < columns; ++x)
                game.processCell(x, y);

        // synchronizacja między iteracjami
        sync_point.arrive_and_wait();

        if (startRow == 0)
            game.applyIteration();

        sync_point.arrive_and_wait();
    }
}

double cpuParallel(GameInstance &game, int threads, int iterations) {
    Timer timer;
    timer.Start();

    int columns = game.getColumns();
    int rows = game.getRows();
    int rowsPerThread = rows / threads;
    int remainder = rows % threads;

    std::barrier sync_point(threads);

    std::vector<std::thread> workers;
    workers.reserve(threads);

    int startRow = 0;
    for (int t = 0; t < threads; ++t) {
        int endRow = startRow + rowsPerThread + (t < remainder ? 1 : 0);
        workers.emplace_back(threadFunc, std::ref(game), startRow, endRow, columns,
                             std::ref(sync_point), iterations);
        startRow = endRow;
    }

    for (auto &w : workers)
        w.join();

    return timer.Stop();
}