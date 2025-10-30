#pragma once

#include <string>
#include "../gameLogic/GameInstance.h"

class VisualizationController{
    private:
        GameInstance& game;
        float animationsSpeed;
        int threads;

        int rows;
        int columns;
        int iteration = 0;

        std::string* colors;
        const std::string ansiFormat = "\033[";
        const std::string black = ansiFormat+"40m";
        const std::string resetC = ansiFormat+"0m";
    public:
        VisualizationController(GameInstance& game, int threads, float animationsSpeed);
        ~VisualizationController();
        void show();
        int whichColor(int row);
        int whichColor(int x, int y);
};
