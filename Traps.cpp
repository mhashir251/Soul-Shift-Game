#include "Traps.hpp"
#include "Settings.hpp"
#include <algorithm>
#include <cmath>

SpikeTrap::SpikeTrap(float px, float py, float w, float h)
    : Trap(px, py, w, h) {}

bool SpikeTrap::isDeadlyTo(bool isSoulForm) const {
    return !isSoulForm;
}

void SpikeTrap::update(float /*dt*/) {}

void SpikeTrap::render(HDC hdc) const {
    HBRUSH brush = CreateSolidBrush(Settings::SPIKE_BASE);
    HPEN pen = CreatePen(PS_SOLID, 2, Settings::SPIKE_TIP);
    HGDIOBJ oldBrush = SelectObject(hdc, brush);
    HGDIOBJ oldPen = SelectObject(hdc, pen);

    int count = std::max(1, static_cast<int>(width / 16.f));
    float spikeW = width / static_cast<float>(count);

    for (int i = 0; i < count; ++i) {
        int x0 = static_cast<int>(x + i * spikeW);
        int x1 = static_cast<int>(x + (i + 0.5f) * spikeW);
        int x2 = static_cast<int>(x + (i + 1.f) * spikeW);
        int yTop = static_cast<int>(y);
        int yBot = static_cast<int>(y + height);

        POINT pts[3] = {{x0, yBot}, {x1, yTop}, {x2, yBot}};
        Polygon(hdc, pts, 3);
    }

    SelectObject(hdc, oldPen);
    SelectObject(hdc, oldBrush);
    DeleteObject(pen);
    DeleteObject(brush);
}

AstralTrap::AstralTrap(float px, float py, float w, float h)
    : Trap(px, py, w, h) {}

bool AstralTrap::isDeadlyTo(bool isSoulForm) const {
    return isSoulForm;
}

void AstralTrap::update(float dt) {
    timer += dt * 4.5f;
}

void AstralTrap::render(HDC hdc) const {
    float wave = 0.5f + 0.5f * std::sin(timer);
    COLORREF outerCol = RGB(
        static_cast<int>(150 + 105 * wave),
        static_cast<int>(25 + 35 * wave),
        static_cast<int>(130 + 60 * wave)
    );

    HBRUSH outerBrush = CreateSolidBrush(outerCol);
    HPEN pen = CreatePen(PS_SOLID, 2, Settings::ASTRAL_CORE);
    HGDIOBJ oldBrush = SelectObject(hdc, outerBrush);
    HGDIOBJ oldPen = SelectObject(hdc, pen);

    RECT r = getBounds().toWinRect();
    Rectangle(hdc, r.left, r.top, r.right, r.bottom);

    HBRUSH coreBrush = CreateSolidBrush(Settings::ASTRAL_CORE);
    SelectObject(hdc, coreBrush);
    int padX = static_cast<int>(width * 0.25f);
    int padY = static_cast<int>(height * 0.25f);
    Rectangle(hdc, r.left + padX, r.top + padY, r.right - padX, r.bottom - padY);

    SelectObject(hdc, oldPen);
    SelectObject(hdc, oldBrush);
    DeleteObject(coreBrush);
    DeleteObject(pen);
    DeleteObject(outerBrush);
}

GoalPortal::GoalPortal(float px, float py)
    : GameObject(px, py, 36.f, 52.f) {}

void GoalPortal::update(float dt) {
    animTimer += dt * 4.f;
}

void GoalPortal::render(HDC hdc) const {
    HBRUSH outerBrush = CreateSolidBrush(Settings::GOAL_OUTER);
    HPEN pen = CreatePen(PS_SOLID, 2, Settings::GOAL_INNER);
    HGDIOBJ oldBrush = SelectObject(hdc, outerBrush);
    HGDIOBJ oldPen = SelectObject(hdc, pen);

    RECT r = getBounds().toWinRect();
    Rectangle(hdc, r.left, r.top, r.right, r.bottom);

    float pulse = 0.5f + 0.5f * std::sin(animTimer);
    int inset = static_cast<int>(7 + 3 * pulse);
    HBRUSH innerBrush = CreateSolidBrush(Settings::GOAL_INNER);
    SelectObject(hdc, innerBrush);
    Rectangle(hdc, r.left + inset, r.top + inset, r.right - inset, r.bottom - inset);

    SelectObject(hdc, oldPen);
    SelectObject(hdc, oldBrush);
    DeleteObject(innerBrush);
    DeleteObject(pen);
    DeleteObject(outerBrush);
}
