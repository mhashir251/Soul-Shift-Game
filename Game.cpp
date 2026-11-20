#include "Game.hpp"
#include "Settings.hpp"
#include <algorithm>
#include <cstdio>

Game::Game()
    : level(buildLevel(1)),
      player(level.spawnX, level.spawnY) {}

bool Game::init(HWND windowHandle) {
    hwnd = windowHandle;
    loadLevel(1);
    return hwnd != nullptr;
}

void Game::loadLevel(int levelNum) {
    level = buildLevel(levelNum);
    player.reset(level.spawnX, level.spawnY);
}

void Game::onKeyDown(WPARAM vkCode) {
    if (vkCode == VK_ESCAPE) {
        running = false;
        if (hwnd) PostMessageW(hwnd, WM_CLOSE, 0, 0);
    } else if (vkCode == VK_SHIFT || vkCode == VK_LSHIFT || vkCode == VK_RSHIFT || vkCode == 'E') {
        player.toggleForm(level.obstacles);
    } else if (vkCode == 'R') {
        loadLevel(currentLevelNum);
    }
}

void Game::update(float dt) {
    player.handleInput(dt);
    player.updatePhysics(dt, level.obstacles);

    for (auto& obs : level.obstacles) obs->update(dt);
    for (auto& trap : level.traps) trap->update(dt);
    if (level.goal) level.goal->update(dt);

    RectF pBounds = player.getBounds();

    for (const auto& trap : level.traps) {
        if (trap->isDeadlyTo(player.isInSoulForm()) && pBounds.intersects(trap->getBounds())) {
            player.reset(level.spawnX, level.spawnY);
            return;
        }
    }

    if (player.getY() > static_cast<float>(Settings::HEIGHT) + 40.f) {
        player.reset(level.spawnX, level.spawnY);
        return;
    }

    if (level.goal && pBounds.intersects(level.goal->getBounds())) {
        currentLevelNum++;
        if (currentLevelNum > totalLevels) {
            currentLevelNum = 1;
        }
        loadLevel(currentLevelNum);
    }
}

void Game::drawBackgroundGrid(HDC hdc) {
    RECT clientRect = {0, 0, Settings::WIDTH, Settings::HEIGHT};
    HBRUSH bgBrush = CreateSolidBrush(Settings::BG_COLOR);
    FillRect(hdc, &clientRect, bgBrush);
    DeleteObject(bgBrush);

    HPEN gridPen = CreatePen(PS_SOLID, 1, Settings::GRID_COLOR);
    HGDIOBJ oldPen = SelectObject(hdc, gridPen);

    for (int x = 24; x < Settings::WIDTH - 24; x += 48) {
        MoveToEx(hdc, x, 24, nullptr);
        LineTo(hdc, x, Settings::HEIGHT - 24);
    }
    for (int y = 24; y < Settings::HEIGHT - 24; y += 48) {
        MoveToEx(hdc, 24, y, nullptr);
        LineTo(hdc, Settings::WIDTH - 24, y);
    }

    SelectObject(hdc, oldPen);
    DeleteObject(gridPen);
}

void Game::renderHUD(HDC hdc) {
    HBRUSH bgBrush = CreateSolidBrush(Settings::HUD_BG);
    HPEN borderPen = CreatePen(PS_SOLID, 2, Settings::HUD_BORDER);
    HGDIOBJ oldB = SelectObject(hdc, bgBrush);
    HGDIOBJ oldP = SelectObject(hdc, borderPen);

    Rectangle(hdc, 38, 36, 242, 54);

    int fillW = static_cast<int>(200.f * player.getEnergyRatio());
    if (fillW > 0) {
        COLORREF barCol = player.isInSoulForm() ? Settings::SOUL_FILL : Settings::HUMAN_FILL;
        HBRUSH fillBrush = CreateSolidBrush(barCol);
        RECT fillRect = {40, 38, 40 + fillW, 52};
        FillRect(hdc, &fillRect, fillBrush);
        DeleteObject(fillBrush);
    }

    SelectObject(hdc, oldP);
    SelectObject(hdc, oldB);
    DeleteObject(borderPen);
    DeleteObject(bgBrush);

    for (int i = 1; i <= totalLevels; ++i) {
        COLORREF pipCol = (i <= currentLevelNum) ? Settings::GOAL_OUTER : RGB(45, 50, 65);
        HBRUSH pipBrush = CreateSolidBrush(pipCol);
        RECT pipRect = {
            Settings::WIDTH - 120 + (i - 1) * 30,
            38,
            Settings::WIDTH - 98 + (i - 1) * 30,
            52
        };
        FillRect(hdc, &pipRect, pipBrush);
        DeleteObject(pipBrush);
    }

    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, Settings::TEXT_COLOR);
    wchar_t hudText[128];
    swprintf(
        hudText,
        128,
        L"FORM: %ls [SHIFT/E]   |   LEVEL %d/%d   |   [R] RESET",
        player.isInSoulForm() ? L"SOUL" : L"HUMAN",
        currentLevelNum,
        totalLevels
    );
    TextOutW(hdc, 260, 37, hudText, static_cast<int>(wcslen(hudText)));
}

void Game::render(HDC hdc) {
    drawBackgroundGrid(hdc);

    for (const auto& obs : level.obstacles) obs->render(hdc);
    for (const auto& trap : level.traps) trap->render(hdc);
    if (level.goal) level.goal->render(hdc);
    player.render(hdc);
    renderHUD(hdc);
}

void Game::stepAndRender(float dt) {
    update(dt);

    if (!hwnd) return;
    HDC windowDC = GetDC(hwnd);
    HDC memDC = CreateCompatibleDC(windowDC);
    HBITMAP memBitmap = CreateCompatibleBitmap(windowDC, Settings::WIDTH, Settings::HEIGHT);
    HGDIOBJ oldBitmap = SelectObject(memDC, memBitmap);

    render(memDC);

    BitBlt(windowDC, 0, 0, Settings::WIDTH, Settings::HEIGHT, memDC, 0, 0, SRCCOPY);

    SelectObject(memDC, oldBitmap);
    DeleteObject(memBitmap);
    DeleteDC(memDC);
    ReleaseDC(hwnd, windowDC);
}
