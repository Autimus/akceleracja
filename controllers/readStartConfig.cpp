#include <iostream>
#include <fstream>
#include <filesystem>
#include "readStartConfig.h"
#include "writeController.h"
#include "../tools/switchCase.h"
#include "../tools/BetterRandom.h"

using namespace std;

void readStartConfig(std::filesystem::path& runningDir, int& columns, int& rows, float& simulationSpeed,std::vector<std::pair<int,int>>& startingCells, bool visualize, string& algorithmName, int& iterations, bool& randomStart){
    string input;
    cout << "Podaj nazwe pliku konfiguracyjnego z folderu '/configFiles' lub nacisnij [ENTER] by konfigurowac recznie:" << endl;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    getline(cin,input);
    bool stop = false;
    while (!input.empty() && !stop) {
        stop = readFile(runningDir,columns,rows,simulationSpeed,startingCells,input);
        if (!stop) { // Plik nie istnieje lub istnieje, ale nie ma danych: kolumny, rzędy i szybkość symulacji.
            cout << "!!!Nie udalo sie wczytac pliku: " << input << endl << "Podaj jeszcze raz nazwe:" << endl;
            getline(cin,input);
        }
    }


    if (input.empty()) {
        // Nie podano nazwy pliku — trzeba wpisać konfigurację ręcznie.
        auto* missingData = new string[3]{"kolumny","rzedy","szybkosc symulacji"};
        int howManyLoops = (visualize)?3:2;
        for (int i=0; i<howManyLoops; i++) {
            cout << "Podaj "+ missingData[i] <<":" << endl;
            float inputFloat;
            cin >> inputFloat;
            if (cin.fail()) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                i--;
            } else {
                switch (i) {
                    case 0:
                        columns = static_cast<int>(inputFloat);
                        break;
                    case 1:
                        rows = static_cast<int>(inputFloat);
                        break;
                    case 2:
                    default:
                        simulationSpeed = inputFloat;
                        break;
                }
            }
        }
        delete[] missingData;

        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        do {
            cout << "Podaj adres poczatkowej komorki np. 2 4 (X Y)"<<endl<<"lub nacisnij [ENTER] by zakonczyc dodawanie:" << endl;
            getline(cin,input);
            if (input.empty())
                break;

            istringstream iss(input);
            if (int x,y; iss >> x >> y) {
                if (x >= columns || y >= rows || x < 0 || y < 0) {
                    std::cout << "Zly zakres. 0 <= X < "<<columns << "; 0 <= Y < " << rows << endl;
                    continue;
                }
                startingCells.emplace_back(x,y);
            } else {
                std::cout << "Niepoprawny format! Wpisz w formacie: X Y." << endl;
            }
        } while (!input.empty());

        do {
            cout << "Podaj liczbe losowych komorek do dodania:"<<endl;
            getline(cin,input);
            if (input.empty()) {
                input="0";
            }
            if (int temp = stoi(input); temp < 0 || temp >= (columns*rows) - startingCells.size() ) {
                input.clear();
            }
        } while (input.empty());

        int howManyCells = stoi(input);
        if (howManyCells > 0){
            int howManyRandomsLeft = howManyCells;
            BetterRandom randX = BetterRandom(0,columns);
            BetterRandom randY = BetterRandom(0,rows);
            while (howManyRandomsLeft > 0) {
                int x = randX.rand();
                int y = randY.rand();
                if (ranges::find(startingCells,pair(x,y)) == startingCells.end()) {
                    startingCells.emplace_back(x,y);
                }
                howManyRandomsLeft--;
            }
            randomStart=false;
        }

        switch (switchCase(new string[3]{"Czy zapisac konfiguracje do pliku?","Tak","Nie"},3)) {
            case 1:
                cout << "Podaj nazwe pliku:" << endl;
                cin>>input;
                writeStartConfig(columns,rows,simulationSpeed,runningDir/"configFiles",addTxtExtension(input),startingCells);
                break;
            default:
            case 2:
                break;
        }
    }
    do {
        cout << "Podaj liczbe iteracji symulacji (-1 oznacza nieskonczonosc):"<<endl;
        getline(cin,input);
        if (int temp = stoi(input); temp == 0 || temp < -1) {
            input.clear();
        }
    } while (input.empty());
    iterations=stoi(input);

    switch (switchCase(new string[5] {
        "Jakim 'algorytmem' chcesz uruchomic symulacje?","Liniowe CPU","Wielowatkowe CPU","GPU ver. 1", "GPU ver. 2"
    },5)) {
        default:
        case 1:
            algorithmName = "cpulinear";
            break;
        case 2:
            algorithmName = "cpuparallel";
            break;
        case 3:
            algorithmName = "gpu1";
            break;
        case 4:
            algorithmName = "gpu2";
            break;

    }
    //TODO: odkomentować poniższy switch-case, i usunąć powyższy switch-case jeżeli będzie potrzebne "gpu3":
    // switch (switchCase(new string[6] {
    //     "Jakim 'algorytmem' chcesz uruchomic symulacje?","Liniowe CPU","Wielowatkowe CPU","GPU ver. 1", "GPU ver. 2","GPU ver. 3"
    // },6)) {
    //     default:
    //     case 1:
    //         algorithmName = "cpulinear";
    //         break;
    //     case 2:
    //         algorithmName = "cpuparallel";
    //         break;
    //     case 3:
    //         algorithmName = "gpu1";
    //         break;
    //     case 4:
    //         algorithmName = "gpu2";
    //         break;
    //     case 5:
    //         algorithmName = "gpu3";
    //         break;
    //
    // }
}

    // Wczytuje konfigurację z pliku.
bool readFile(std::filesystem::path& runningDir, int& columns, int& rows, float& simulationSpeed,std::vector<std::pair<int,int>>& startingCells, string& fileName) {
    ifstream file;
    fileName = addTxtExtension(fileName);
    // Próbuje wczytać dane z pliku.
    file = ifstream(runningDir/"configFiles"/fileName);
    int fileLength = 0;
    for (string _; getline(file, _); ++fileLength) {}
    file.clear();
    file.seekg(0, ifstream::beg);
    if (fileLength < 3)
        return false;

    string line;
    file >> columns >> rows >> simulationSpeed;
    file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    while (getline(file, line)) {
        cout << line << endl;
        istringstream iss(line);
        if (int x, y; iss >> x >> y) {
            startingCells.emplace_back(x, y);
        }
    }
    file.close();
    return true;
}

std::string addTxtExtension(std::string &filename) {
    std::string result = filename; // robimy kopię

    try {
        if (result.length() < 4 || result.substr(result.length() - 4, 4) != ".txt")
            result.append(".txt");
    } catch (const std::out_of_range&) {
        result.append(".txt");
    }

    return result;
}