#pragma once

#include <vector>
#include <string>

//Wczytuje konfigurację startową z pliku lub z terminala,
//modyfikuje zmienne wsadzone w argumentach i nic nie zwraca,
//zawiera pytania i weryfikację poprawności.
void readStartConfig(std::filesystem::path& runningDir, int& columns, int& rows, float& simulationSpeed,std::vector<std::pair<int,int>>& startingCells, bool visualize, std::string& algorithmName);

// sprawdza, czy nazwa pliku kończy się na .txt, jak nie to go dodaje
std::string addTxtExtension(const std::string &filename);