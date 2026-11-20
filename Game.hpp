#pragma once
#include "Player.hpp"
#include "Level.hpp"
#include <windows.h>

class Game {
private:
    HWND hwnd = nullptr;
    Level level;
    Player player;
    int currentLevelNum = 1;
    const int totalLevels = 3;
    bool running = true;

    void loadLevel(int levelNum);
    void update(float dt);
    void drawBackgroundGrid(HDC hdc);
    void renderHUD(HDC hdc);
    void render(HDC hdc);

public:
    Game();
    bool init(HWND windowHandle);
    void onKeyDown(WPARAM vkCode);
    void onClose() { running = false; }
    bool isRunning() const { return running; }
    void stepAndRender(float dt);
};
