#include "Obstacles.hpp"
#include "Settings.hpp"
#include <cmath>

SolidWall::SolidWall(float px, float py, float w, float h)
    : Obstacle(px, py, w, h) {}

bool SolidWall::blocksPlayer(bool /*isSoulForm*/) const {
    return true;
}

void SolidWall::update(float /*dt*/) {}

void SolidWall::render(HDC hdc) const {
    HBRUSH fillBrush = CreateSolidBrush(Settings::WALL_FILL);
    HPEN pen = CreatePen(PS_SOLID, 2, Settings::WALL_OUTLINE);
    HGDIOBJ oldBrush = SelectObject(hdc, fillBrush);
    HGDIOBJ oldPen = SelectObject(hdc, pen);

    RECT r = getBounds().toWinRect();
    Rectangle(hdc, r.left, r.top, r.right, r.bottom);

    SelectObject(hdc, oldPen);
    SelectObject(hdc, oldBrush);
    DeleteObject(pen);
    DeleteObject(fillBrush);
}

SpiritBarrier::SpiritBarrier(float px, float py, float w, float h)
    : Obstacle(px, py, w, h) {}

bool SpiritBarrier::blocksPlayer(bool isSoulForm) const {
    return !isSoulForm;
}

void SpiritBarrier::update(float dt) {
    pulseTimer += dt * 3.5f;
}

void SpiritBarrier::render(HDC hdc) const {
    float wave = 0.5f + 0.5f * std::sin(pulseTimer);
    COLORREF inner = RGB(
        static_cast<int>(90 + 65 * wave),
        static_cast<int>(45 + 35 * wave),
        static_cast<int>(180 + 75 * wave)
    );

    HBRUSH fillBrush = CreateSolidBrush(inner);
    HPEN pen = CreatePen(PS_SOLID, 2, Settings::BARRIER_OUTLINE);
    HGDIOBJ oldBrush = SelectObject(hdc, fillBrush);
    HGDIOBJ oldPen = SelectObject(hdc, pen);

    RECT r = getBounds().toWinRect();
    Rectangle(hdc, r.left, r.top, r.right, r.bottom);

    SelectObject(hdc, oldPen);
    SelectObject(hdc, oldBrush);
    DeleteObject(pen);
    DeleteObject(fillBrush);
}
