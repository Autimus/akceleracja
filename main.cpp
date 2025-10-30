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
#include "controllers/VisualizationController.h"
#include "gameLogic/GameInstance.h"
#include "algorithms/cpuLinear.h"
#include "algorithms/cpuParallel.h"
#include "algorithms/gpuBasic.h"

using namespace std;
namespace fs = std::filesystem;

int main() {
    fs::path runningDir = fs::current_path().parent_path().parent_path();

    vector<pair<int,int>> startingCells;
    EnvVar envVar;

    // Dane, które można wbić na sztywno, jak się nie chce za każdym razem klikać w terminalu. Ustaw wtedy "skipQuestions = true":
    // Po pierwszym uruchomieniu programu, tworzy plik .env, który przechowuje zmienne środowiskowe i ich modyfikacja modyfikuje,
    // działanie aplikacji.
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
        readStartConfig(runningDir, columns, rows, simulationSpeed,startingCells ,visualize,algorithmName, iterations, randomStart);
    } else {
        readFile(runningDir, columns, rows, simulationSpeed, startingCells, filename);
    }
    GameInstance game(columns,rows,startingCells);
    if (randomStart) {
        game.addRandoms(howManyRandoms);
    }

    cout << "Rozpoczynam symulację: "<<columns<<" x " << rows;
    if (visualize) {
        cout<<"; prędkość: "<<simulationSpeed;
    }
    cout<<endl;
    game.print();


    int cpuThreads = thread::hardware_concurrency();
    int gpuThreads = countGPUThreads();
    VisualizationController terminal(game, (algorithmName.substr(0,3) == "cpu")?cpuThreads:gpuThreads, simulationSpeed);

    if (algorithmName == "cpulinear") {
        if (visualize) {
            for (int i = 0; i < iterations; i++) {
                cpuLinear(game);
                terminal.show();
            }
        } else {
            double time = cpuLinear(game, iterations);
            cout<<"Czas wykonywania: "<<time<<"[s]"<<endl;
            saveStatistics(1,columns*rows,time,algorithmName,runningDir/"results");
        }
    } else if (algorithmName == "cpuparallel") {
        if (visualize) {
            for (int i = 0; i < iterations; i++) {
                cpuParallel(game,cpuThreads);
                terminal.show();
            }
        } else {
            double time = cpuParallel(game,cpuThreads ,iterations);
            cout<<"Czas wykonywania: "<<time<<"[s]"<<endl;
            saveStatistics(cpuThreads,columns*rows,time,algorithmName,runningDir/"results");
        }
    } else if (algorithmName == "gpu1") {
        //GPU 1: funkcja lub kod.
        if (visualize) {
            for(int i=0;i<iterations;i++){
                gpuBasic(game);
                visualizationController(game, simulationSpeed, i);
            }
        }else {
            double time = gpuBasic(game, iterations);
            cout << "Czas wykonywania: " << time << "[s]" << endl;
            //liczba wątków wykorzystana na jedną iterację
            int blocksX = (columns + 16 - 1) / 16;
            int blocksY = (rows + 16 - 1) / 16;
            int totalBlocks = blocksX * blocksY;
            int threadsPerBlock = 16 * 16; // 256
            int totalThreads = totalBlocks * threadsPerBlock;
            saveStatistics(totalThreads,columns*rows,time,algorithmName,runningDir/"results");

        }
    } else if (algorithmName == "gpu2") {
        //GPU 2: funkcja lub kod.
    } else if (algorithmName == "gpu3") {
        //GPU 3: funkcja lub kod.
    }

    return 0;
}