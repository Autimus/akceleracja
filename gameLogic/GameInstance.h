#pragma once
#include <vector>

class GameInstance {
private:
    int columns;
    int rows;
    bool** gameArea;
    bool** nextIteration;
    std::pair<int,int>* whatIsNeighbour;
public:
    GameInstance(int columns, int rows, std::vector<std::pair<int,int>>& startingCells);
    ~GameInstance();

    int howManyNeighbours(int x, int y);
    void processCell(int x, int y);
    void applyIteration();

    void addRandoms(int howManyCells);
    bool** getGameArea();
    int getColumns();
    int getRows();
    void print();
    std::vector <std::pair<int,int>> toVector();
    GameInstance copy();
};