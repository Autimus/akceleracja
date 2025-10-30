#include "VisualizationController.h"
#include "../tools/clearScreen.h"
#include <chrono>
#include <iostream>
#include <thread>

#include "../tools/BetterRandom.h"

using namespace std;

VisualizationController::VisualizationController(GameInstance& game, int threads, float animationsSpeed)
    : game(game),
    threads(threads),
    animationsSpeed(animationsSpeed) {

    this -> columns = game.getColumns();
    this -> rows = game.getRows();

    BetterRandom random(0,256);
    this->colors = new string[threads]; //ansiFormat+"4",ansiFormat+"10"
    int i = 0;
    while (i < threads) {
        if (i < 7) {
            colors[i] = ansiFormat+"4"+to_string(i+1)+"m";
        } else if (i < 15) {
            colors[i] = ansiFormat+"10"+to_string(i-7)+"m";
        } else if (i < 15 + 256) {
            colors[i] = ansiFormat+"48;5;"+to_string(i-15)+"m";
        } else {
            colors[i] = ansiFormat + "48;2;" + to_string(random.rand()) + ";" + to_string(random.rand()) + ";" + to_string(random.rand()) + "]";
        }

        i++;
    }
}

VisualizationController::~VisualizationController() {
    delete[] colors;
}


void VisualizationController::show(){
    clearScreen();
    bool** gameArea = game.getGameArea();

    for (int i = 0; i < rows; i++) {
        int threadNumber = whichColor(i);
        for (int j = 0; j < columns; j++) {
            string color = (gameArea[i][j]) ? colors[threadNumber] : black;
            cout << color << "  ";
        }
        cout << resetC << endl;
    }

    cout<< resetC <<"Iteracja: "<<iteration++<<endl;
    this_thread::sleep_for(chrono::milliseconds(static_cast<int>((1000/animationsSpeed))));
}

int VisualizationController::whichColor(int row) {
    int rowsPerThread = rows / threads;
    int remainder = rows % threads;
    int cutoff = (rowsPerThread + 1) * remainder;

    return (row < cutoff)? row / (rowsPerThread + 1) : remainder + (row - cutoff) / rowsPerThread;
}