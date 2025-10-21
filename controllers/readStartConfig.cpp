#include <iostream>
#include <fstream>
#include <filesystem>
#include "readStartConfig.h"
using namespace std;

void readStartConfig(std::filesystem::path& runningDir, int& columns, int& rows, float& simulationSpeed,std::vector<std::pair<int,int>>& startingCells, bool visualize){
    string input;
        cout << "Podaj nazwę pliku konfiguracyjnego z folderu '/configFiles' lub naciśnij [ENTER] by konfigurować ręcznie:" << endl;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        getline(cin,input);
        while (!input.empty() || columns * rows * static_cast<int>(simulationSpeed) != 0) {
            try { // sprawdza, czy nazwa pliku kończy się na .txt.
                if (input.substr(input.length() - 4,4) != ".txt")
                    input.append(".txt");
            } catch (const out_of_range& e) {
                input.append(".txt");
            }
            // Próbuje wczytać dane z pliku.
            ifstream file(runningDir/"configFiles"/input);
            int fileLength = 0;
            for (std::string _; std::getline(file, _); ++fileLength) {}
            file.seekg(0, ifstream::beg);
            if (fileLength < 3) { // Plik nie istnieje lub istnieje, ale nie ma danych: kolumny, rzędy i szybkość symulacji.
                cout << "!!!Nie udało się wczytać pliku: " << input << endl << "Podaj jeszcze raz nazwę:" << endl;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                getline(cin,input);
                continue;
            }

            // Zczytuje konfigurację z pliku.
            //TODO: zczytywanie z pliku ,columns, rows, speed, startingCells.push_back({1,2]);
            file.close();
        }
        if (input.empty()) { // Nie podano nazwy pliku — trzeba wpisać konfigurację ręcznie.
            string* missingData = new string[3]{"kolumny","rzędy","szybkość symulacji"};
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
                            simulationSpeed = inputFloat;
                            break;
                    }
                }
            }
            delete[] missingData;
            do {
                cout << "Podaj adres początkowej komórki np. 2 4 (X Y)"<<endl<<"lub naciśnij [ENTER] by zakończyć dodawanie:" << endl;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                getline(cin,input);
                if (!input.empty())
                    break;

                istringstream iss(input);
                if (int x,y; iss >> x >> y) {

                } else {
                    std::cout << "Niepoprawny format! Wpisz w formacie: X Y.\n";
                }
            } while (!input.empty());
        }
}