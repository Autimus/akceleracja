#pragma once
#include <barrier>

#include "../gameLogic/GameInstance.h"

// Wątek, który wykonuje iterację zadanego zakresu rzędów i może być wywołana ponownie.


// Przeprowadza 1 iterację game of life bez pomiaru czasu. Edytuje argument "game" na potrzeby wizualizacji.
void cpuParallel(GameInstance& game, int threads);

// Funkcja wywoływana przez wątki pozwala na wykonywanie kolejnych iteracji bez potrzeby deklarowania na nowo wątków,
// Przyspieszenie względem wielokrotnego wywoływania "cpuParallel (GameInstance& game, int threads)" ~ 7%
void threadFunc(GameInstance &game, int startRow, int endRow, int columns, std::barrier<> &sync_point, int iterations);

// Przeprowadza game of life z pomiarem czasu i bez wizualizacji, zwraca czas wykonywania [s].
double cpuParallel(GameInstance& game, int threads, int iterations);
