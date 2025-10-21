#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include "tools/switchCase.h"
#include "tools/Timer.h"
#include "controllers/readStartConfig.h"

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
    bool** gameArea = nullptr;

    // Dane, które można wbić na sztywno, jak się nie chce za każdym razem klikać w terminalu. Ustaw wtedy "skipQuestions = true":
    bool skipQuestions = false;
    bool visualize = false;
    string filename = "";
    // Nie trzeba deklarować, jeżeli podano "filename".
    int columns = 0, rows = 0;
    // Zmienna używana tylko przy trybie graficznym. Nie trzeba deklarować, jeżeli podano "filename".
    float simulationSpeed = 0;
    // Nie trzeba deklarować, jeżeli podano "filename".
    vector<pair<int,int>> startingCells;
    //startingCells.emplace_back(0, 0);

    if (!skipQuestions) {
        switch (switchCase(new string[3]{"Wybierz tryb pracy","Graficzny","Tekstowy - bez wizualizacji, ale z pomiarem czasu"},3)) {
            case 1:
                visualize = true;
                break;
            case 2:
                visualize = false;
                break;
        }
        readStartConfig(runningDir, columns, rows, simulationSpeed,startingCells ,visualize);
    }
    gameArea = new bool*[columns];
    for (int i = 0; i < columns; i++) {
        gameArea[i] = new bool[rows];
        for (int j = 0; j < rows; j++) {
            gameArea[i][j] = false;
        }
    }
    cout << "Rozpoczynam symulację: "<<columns<<" x " << rows<< "; prędkość: "<<simulationSpeed<<endl;
    for (int i = 0; i < columns; i++) {
        for (int j = 0; j < rows; j++) {
            cout<<gameArea[i][j]<<" ";
        }
        cout<<endl;
    }

    //TODO: uruchomienie koknretnej symulacji CPU lub GPU, pomiar czasu lub wyświetlanie symulacji

    //Zwalnianie pamięci.
    for (int i = 0; i < columns; i++) {
        delete[] gameArea[i];
    }
    delete[] gameArea;
    return 0;
}