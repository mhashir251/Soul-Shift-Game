#pragma once
#include <windows.h>
#include <cstdint>

namespace Settings {
    constexpr int WIDTH = 960;
    constexpr int HEIGHT = 600;
    constexpr int FPS = 60;
    constexpr const wchar_t* TITLE = L"SOUL SHIFT - C++ Win32 OOP Edition";

    // Colors (0x00BBGGRR format for Windows GDI COLORREF)
    constexpr COLORREF BG_COLOR = RGB(18, 20, 28);
    constexpr COLORREF GRID_COLOR = RGB(26, 29, 40);

    constexpr COLORREF WALL_FILL = RGB(48, 54, 72);
    constexpr COLORREF WALL_OUTLINE = RGB(76, 86, 114);

    constexpr COLORREF BARRIER_FILL = RGB(145, 70, 255);
    constexpr COLORREF BARRIER_OUTLINE = RGB(205, 145, 255);

    constexpr COLORREF SPIKE_BASE = RGB(180, 30, 45);
    constexpr COLORREF SPIKE_TIP = RGB(255, 80, 95);

    constexpr COLORREF ASTRAL_FILL = RGB(255, 45, 180);
    constexpr COLORREF ASTRAL_CORE = RGB(255, 130, 225);

    constexpr COLORREF GOAL_OUTER = RGB(45, 220, 135);
    constexpr COLORREF GOAL_INNER = RGB(150, 255, 205);

    constexpr COLORREF HUMAN_FILL = RGB(55, 200, 255);
    constexpr COLORREF HUMAN_OUTLINE = RGB(210, 245, 255);

    constexpr COLORREF SOUL_FILL = RGB(185, 110, 255);
    constexpr COLORREF SOUL_OUTLINE = RGB(242, 215, 255);

    constexpr COLORREF HUD_BG = RGB(25, 28, 38);
    constexpr COLORREF HUD_BORDER = RGB(115, 125, 155);
    constexpr COLORREF TEXT_COLOR = RGB(225, 232, 245);

    // Physics & Gameplay Tuning
    constexpr float PLAYER_WIDTH = 28.f;
    constexpr float PLAYER_HEIGHT = 38.f;

    constexpr float HUMAN_SPEED = 270.f;
    constexpr float SOUL_SPEED = 315.f;

    constexpr float HUMAN_GRAVITY = 1350.f;
    constexpr float SOUL_GRAVITY = 320.f;

    constexpr float HUMAN_JUMP_VEL = -540.f;
    constexpr float SOUL_FLOAT_ACCEL = 920.f;
    constexpr float SOUL_MAX_RISE = -240.f;

    constexpr float MAX_FALL_HUMAN = 800.f;
    constexpr float MAX_FALL_SOUL = 220.f;

    constexpr float MAX_SOUL_ENERGY = 100.f;
    constexpr float SOUL_DRAIN_RATE = 22.f;
    constexpr float SOUL_RECHARGE_RATE = 28.f;
    constexpr float MIN_SHIFT_ENERGY = 15.f;
}
