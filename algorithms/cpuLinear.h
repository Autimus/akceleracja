#pragma once
#include "../gameLogic/GameInstance.h"

// Przeprowadza 1 iterację game of life bez pomiaru czasu. Edytuje argument "game" na potrzeby wizualizacji.
void cpuLinear(GameInstance& game);

// Przeprowadza game of life z pomiarem czasu i bez wizualizacji, zwraca czas wykonywania [s].
double cpuLinear(GameInstance& game, int iterations);
