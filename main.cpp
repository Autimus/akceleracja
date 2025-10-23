#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <thread>

#include "tools/switchCase.h"
#include "tools/EnvVar.h"
#include "tools/countGPUThreads.h"
#include "controllers/readStartConfig.h"
#include "controllers/writeController.h"
#include "controllers/visualizationController.h"
#include "gameLogic/GameInstance.h"
#include "algorithms/cpuLinear.h"

#ifdef _WIN32
#include <conio.h>
#include <cstdlib>
#else
#include <unistd.h>
#include <termios.h>
#endif

using namespace std;
namespace fs = std::filesystem;

int main() {
    fs::path runningDir = fs::current_path().parent_path();
    vector<pair<int,int>> startingCells;
    EnvVar envVar;

    // Dane, które można wbić na sztywno, jak się nie chce za każdym razem klikać w terminalu. Ustaw wtedy "skipQuestions = true":
    bool skipQuestions = envVar.findBool("skipQuestions");
    bool visualize = envVar.findBool("visualize");
    bool randomStart = envVar.findBool("randomStart");
    //Ile losowych komórek startowych wygenerować.
    int howManyRandoms = envVar.findInt("howManyRandoms");
    //Nie trzeba deklarować, jeżeli "randomStart = true".
    string filename = envVar.find("filename");
    // Nie trzeba deklarować, jeżeli podano "filename".
    int columns = envVar.findInt("columns"), rows = envVar.findInt("rows");
    // Zmienna używana tylko przy trybie graficznym. Nie trzeba deklarować, jeżeli podano "filename".
    float simulationSpeed = envVar.findFloat("simulationSpeed");
    string algorithmName = envVar.find("algorithmName");
    // Ile iteracji do zakończenia symulacji. (-1 = nieskończoność)
    int iterations = envVar.findInt("iterations");
    envVar.print();

    if (!skipQuestions) {
        switch (switchCase(new string[3]{"Wybierz tryb pracy","Graficzny","Tekstowy - bez wizualizacji, ale z pomiarem czasu"},3)) {
            case 1:
                visualize = true;
                break;
            case 2:
            default:
                visualize = false;
                break;
        }
        readStartConfig(runningDir, columns, rows, simulationSpeed,startingCells ,visualize,algorithmName, iterations);
    } else {
        readFile(runningDir, columns, rows, simulationSpeed, startingCells, filename);
    }
    GameInstance game(columns,rows,startingCells);
    if (randomStart) {
        game.addRandoms(howManyRandoms);
    }

    cout << "Rozpoczynam symulację: "<<columns<<" x " << rows<< "; prędkość: "<<simulationSpeed<<endl;
    game.print();


    int cpuThreads = thread::hardware_concurrency();
    //int gpuThreads = countGPUThreads();

    if (algorithmName == "cpulinear") {
        if (visualize) {
            for (int i = 0; i < iterations; i++) {
                //clrscr();
                system("clear");
                game.print();
                cout<<"Iteracja: "<<i<<endl;
                cpuLinear(game);
                sleep(1 * simulationSpeed);
            }
        } else {
            double time = cpuLinear(game, iterations);
            saveStatistics(1,columns*rows,time,algorithmName,"results/"+algorithmName+".txt");
        }
    } else if (algorithmName == "cpuparallel") {
        //CPU Parallel: funkcja lub kod.
    } else if (algorithmName == "gpu1") {
        //GPU 1: funkcja lub kod.
    } else if (algorithmName == "gpu2") {
        //GPU 2: funkcja lub kod.
    } else if (algorithmName == "gpu3") {
        //GPU 3: funkcja lub kod.
    }

    return 0;
}