#pragma once
#include "Obstacles.hpp"
#include "Traps.hpp"
#include <vector>
#include <memory>

struct Level {
    int number = 1;
    float spawnX = 70.f;
    float spawnY = 510.f;
    std::vector<std::unique_ptr<Obstacle>> obstacles;
    std::vector<std::unique_ptr<Trap>> traps;
    std::unique_ptr<GoalPortal> goal;
};

Level buildLevel(int levelNum);
