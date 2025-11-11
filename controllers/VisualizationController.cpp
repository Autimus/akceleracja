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
        if (i < 6) {
            colors[i] = ansiFormat+"4"+to_string(i+1)+"m";
        } else if (i < 14) {
            colors[i] = ansiFormat+"10"+to_string(i-6)+"m";
        } else if (i < 14 + 256) {
            colors[i] = ansiFormat+"48;5;"+to_string(i-14)+"m";
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

    // Pozioma biała kreska (ramka canvy)
    for (int i = 0; i < columns + 2; i++) {
        cout << white << "  ";
    }
    cout << resetC << endl;

    for (int i = 0; i < rows; i++) {
        // Pionowa biała kreska (ramka canvy)
        cout << white << "  ";

        int threadNumber;
        if (threads <= 128)
            threadNumber= whichColor(i);
        for (int j = 0; j < columns; j++) {
            if (threads > 128)
                threadNumber= whichColor(j,i);
            string color = (gameArea[i][j]) ? colors[threadNumber] : black;
            cout << color << "  ";
        }

        // Pionowa biała kreska (ramka canvy)
        cout << white << "  ";

        cout << resetC << endl;
    }

    // Pozioma biała kreska (ramka canvy)
    for (int i = 0; i < columns + 2; i++) {
        cout << white << "  ";
    }
    cout << resetC << endl;

    cout<< resetC <<"Iteracja: "<<iteration++<<endl;
    this_thread::sleep_for(chrono::milliseconds(static_cast<int>((1000/animationsSpeed))));
}

// Wątek posiada cały rząd/lub kilka rzędów
int VisualizationController::whichColor(int row) {
    int rowsPerThread = rows / threads;
    int remainder = rows % threads;
    int cutoff = (rowsPerThread + 1) * remainder;

    return (row < cutoff)? row / (rowsPerThread + 1) : remainder + (row - cutoff) / rowsPerThread;
}

// 1 wątek == 1 pixel
int VisualizationController::whichColor(int x, int y) {
    return y * columns + x;
}