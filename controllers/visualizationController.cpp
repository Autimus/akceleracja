#include "visualizationController.h"
#include "../tools/clearScreen.h"
#include <chrono>
#include <iostream>
#include <thread>

using namespace std;

void visualizationController(GameInstance& game, float animationsSpeed, int iteration) {
    clearScreen();
    game.print();
    cout<<"Iteracja: "<<iteration<<endl;
    this_thread::sleep_for(chrono::milliseconds(static_cast<int>((1000/animationsSpeed))));
}