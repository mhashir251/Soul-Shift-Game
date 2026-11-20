#include "Level.hpp"
#include "Settings.hpp"

namespace {
    void addOuterBorders(std::vector<std::unique_ptr<Obstacle>>& obstacles) {
        const float w = static_cast<float>(Settings::WIDTH);
        const float h = static_cast<float>(Settings::HEIGHT);
        obstacles.push_back(std::make_unique<SolidWall>(0.f, 0.f, w, 24.f));
        obstacles.push_back(std::make_unique<SolidWall>(0.f, h - 24.f, w, 24.f));
        obstacles.push_back(std::make_unique<SolidWall>(0.f, 0.f, 24.f, h));
        obstacles.push_back(std::make_unique<SolidWall>(w - 24.f, 0.f, 24.f, h));
    }
}

Level buildLevel(int levelNum) {
    Level lvl;
    lvl.number = levelNum;
    addOuterBorders(lvl.obstacles);

    if (levelNum == 1) {
        lvl.spawnX = 70.f;
        lvl.spawnY = 510.f;
        lvl.obstacles.push_back(std::make_unique<SolidWall>(24.f, 552.f, 912.f, 24.f));
        lvl.obstacles.push_back(std::make_unique<SolidWall>(310.f, 24.f, 28.f, 410.f));
        lvl.obstacles.push_back(std::make_unique<SpiritBarrier>(310.f, 434.f, 28.f, 118.f));
        lvl.traps.push_back(std::make_unique<SpikeTrap>(460.f, 532.f, 160.f, 20.f));
        lvl.obstacles.push_back(std::make_unique<SolidWall>(720.f, 24.f, 28.f, 410.f));
        lvl.obstacles.push_back(std::make_unique<SpiritBarrier>(720.f, 434.f, 28.f, 118.f));
        lvl.goal = std::make_unique<GoalPortal>(850.f, 500.f);
        return lvl;
    }

    if (levelNum == 2) {
        lvl.spawnX = 70.f;
        lvl.spawnY = 500.f;
        lvl.obstacles.push_back(std::make_unique<SolidWall>(24.f, 552.f, 260.f, 24.f));
        lvl.obstacles.push_back(std::make_unique<SolidWall>(360.f, 470.f, 200.f, 24.f));
        lvl.obstacles.push_back(std::make_unique<SolidWall>(640.f, 380.f, 296.f, 24.f));
        lvl.traps.push_back(std::make_unique<AstralTrap>(420.f, 390.f, 80.f, 80.f));
        lvl.obstacles.push_back(std::make_unique<SolidWall>(740.f, 24.f, 26.f, 240.f));
        lvl.obstacles.push_back(std::make_unique<SpiritBarrier>(740.f, 264.f, 26.f, 116.f));
        lvl.traps.push_back(std::make_unique<SpikeTrap>(284.f, 556.f, 652.f, 20.f));
        lvl.goal = std::make_unique<GoalPortal>(860.f, 328.f);
        return lvl;
    }

    lvl.spawnX = 65.f;
    lvl.spawnY = 140.f;
    lvl.obstacles.push_back(std::make_unique<SolidWall>(24.f, 200.f, 220.f, 24.f));
    lvl.obstacles.push_back(std::make_unique<SpiritBarrier>(340.f, 24.f, 24.f, 240.f));
    lvl.obstacles.push_back(std::make_unique<SolidWall>(310.f, 264.f, 180.f, 24.f));
    lvl.traps.push_back(std::make_unique<AstralTrap>(410.f, 184.f, 70.f, 80.f));
    lvl.obstacles.push_back(std::make_unique<SolidWall>(530.f, 440.f, 406.f, 24.f));
    lvl.traps.push_back(std::make_unique<SpikeTrap>(560.f, 420.f, 140.f, 20.f));
    lvl.obstacles.push_back(std::make_unique<SpiritBarrier>(760.f, 280.f, 24.f, 160.f));
    lvl.obstacles.push_back(std::make_unique<SolidWall>(760.f, 24.f, 24.f, 256.f));
    lvl.traps.push_back(std::make_unique<SpikeTrap>(24.f, 556.f, 912.f, 20.f));
    lvl.goal = std::make_unique<GoalPortal>(865.f, 388.f);
    return lvl;
}
