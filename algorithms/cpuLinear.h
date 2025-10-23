#pragma once

// Przeprowadza 1 iterację game of life bez pomiaru czasu. Edytuje argument "gameArea" na potrzeby wizualizacji.
void cpuLinear(int columns, int rows, bool**& gameArea);

// Przeprowadza game of life z pomiarem czasu i bez wizualizacji, zwraca czas wykonywania [s].
double cpuLinear(int columns, int rows, bool** startingCells, int iterations);
