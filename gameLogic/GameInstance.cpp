#include <iostream>

#include "../tools/BetterRandom.h"
#include "GameInstance.h"

using namespace std;

GameInstance::GameInstance(int columns, int rows, vector<pair<int,int>>& startingCells) {
    this->columns = columns;
    this->rows = rows;
    this->gameArea = new bool*[columns];
    for (int i = 0; i < rows; i++) {
        this->gameArea[i] = new bool[rows];
        for (int j = 0; j < columns; j++) {
            this->gameArea[i][j] = false;
        }
    }
    for (auto [x, y] : startingCells) {
        gameArea[y][x] = true;
    }

    this->nextIteration = new bool*[columns];
    for (int i = 0; i < rows; i++) {
        this->nextIteration[i] = new bool[rows];
        for (int j = 0; j < columns; j++) {
            this->nextIteration[i][j] = gameArea[i][j];
        }
    }

    whatIsNeighbour = new pair<int, int>[8] {
        {-1,-1}, {0, -1}, {1,-1},
        {-1,0},                {1,0},
        {-1,1}, {0, 1}, {1, 1}
    };
}

GameInstance::~GameInstance() {
    for (int i = 0; i < rows; i++) {
        delete[] gameArea[i];
    }
    delete[] gameArea;
    delete[] whatIsNeighbour;
}

void GameInstance::addRandoms(const int howManyCells) {
    int howManyEmptyCells = 0;
    for (int x = 0; x < columns; x++) {
        for (int y = 0; y < rows; y++) {
            if (!gameArea[y][x])
                howManyEmptyCells++;
        }
    }

    int howManyRandomsLeft = howManyCells;
    BetterRandom randX = BetterRandom(0,columns);
    BetterRandom randY = BetterRandom(0,rows);
    while (howManyRandomsLeft * howManyRandomsLeft > 0) {
        int x = randX.rand();
        int y = randY.rand();
        if (!gameArea[y][x]) {
            gameArea[y][x] = true;
            howManyRandomsLeft--;
            howManyEmptyCells--;
        }
    }
}

bool** GameInstance::getGameArea() {
    return gameArea;
}

int GameInstance::getColumns() {
    return columns;
}

int GameInstance::getRows() {
    return rows;
}

int GameInstance::howManyNeighbours(int x, int y) {
    int output = 0;
    for (int i = 0; i < 8; i++) {
        auto [dx, dy] = whatIsNeighbour[i];
        int nx = x + dx;
        int ny = y + dy;
        if (nx >= 0 && nx < columns && ny >= 0 && ny < rows) {
            output += gameArea[ny][nx] ? 1 : 0;
        }
    }
    return output;
}



void GameInstance::processCell(int x, int y) {
    if (const int neighbours = howManyNeighbours(x, y); neighbours == 3) {
        nextIteration[y][x] = true;
    } else if (neighbours < 2 || neighbours > 3) {
        nextIteration[y][x] = false;
    }
}

void GameInstance::applyIteration() {
    for (int y = 0; y < rows; y++) {
        for (int x = 0; x < columns; x++) {
            gameArea[y][x] = nextIteration[y][x];
        }
    }
}

void GameInstance::print() {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++) {
            cout<<((gameArea[i][j])?"1":"0");
        }
        cout<<endl;
    }
}




vector <std::pair<int,int>> GameInstance::toVector() {
    vector <std::pair<int,int>> output;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++) {
            if (gameArea[i][j]) {
                output.emplace_back(j,i);
            }
        }
    }
    return output;
}

GameInstance GameInstance::copy() {
    vector <std::pair<int,int>> startingCells = toVector();
    GameInstance gameCopy = GameInstance(columns,rows,startingCells);
    return gameCopy;
}
