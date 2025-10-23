#pragma once

#include "../tools/BetterRandom.h"

//Edytuje argument "gameArea" wstawiając "howManyCells" wartości 'true'.
inline void generateRandomGame(const int columns, const int rows ,bool**& gameArea, const int howManyCells) {
    int howManyEmptyCells = 0;
    for (int x = 0; x < columns; x++) {
        for (int y = 0; y < rows; y++) {
            if (!gameArea[x][y])
                howManyEmptyCells++;
        }
    }

    int howManyRandomsLeft = howManyCells;
    BetterRandom randX = BetterRandom(0,columns);
    BetterRandom randY = BetterRandom(0,rows);
    while (howManyRandomsLeft * howManyRandomsLeft > 0) {
        int x = randX.rand();
        int y = randY.rand();
        if (!gameArea[x][y]) {
            gameArea[x][y] = true;
            howManyRandomsLeft--;
            howManyEmptyCells--;
        }
    }
}