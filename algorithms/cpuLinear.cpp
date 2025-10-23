#include "cpuLinear.h"
#include "../tools/Timer.h"

using namespace std;

void cpuLinear(GameInstance& game) {
    int columns = game.getColumns();
    int rows = game.getRows();
    for (int y=0; y<rows; y++) {
        for (int x=0; x<columns; x++) {
            game.processCell(x,y);
        }
    }
    game.applyIteration();
}

double cpuLinear(GameInstance& game, int iterations) {
    Timer timer;
    int columns = game.getColumns();
    int rows = game.getColumns();
    timer.Start(); // Start pomiaru

    for (int i = 0; i < iterations; i++) {
        for (int x=0; x<rows; x++) {
            for (int y=0; y<columns; y++) {
                game.processCell(x,y);
            }
        }
        game.applyIteration();
    }

    const double output = timer.Stop(); // Stop pomiaru
    return output;
}