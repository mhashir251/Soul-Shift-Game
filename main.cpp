#include "Game.hpp"
#include "Settings.hpp"
#include <windows.h>
#include <algorithm>

namespace {
    Game* g_game = nullptr;

    LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
        switch (msg) {
            case WM_KEYDOWN:
                if (g_game) g_game->onKeyDown(wParam);
                return 0;
            case WM_DESTROY:
                if (g_game) g_game->onClose();
                PostQuitMessage(0);
                return 0;
            default:
                return DefWindowProcW(hwnd, msg, wParam, lParam);
        }
    }
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int nCmdShow) {
    WNDCLASSW wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"SoulShiftWin32Class";
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);

    RegisterClassW(&wc);

    RECT wr = {0, 0, Settings::WIDTH, Settings::HEIGHT};
    AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX, FALSE);

    HWND hwnd = CreateWindowExW(
        0,
        wc.lpszClassName,
        Settings::TITLE,
        WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        wr.right - wr.left,
        wr.bottom - wr.top,
        nullptr,
        nullptr,
        hInstance,
        nullptr
    );

    if (!hwnd) return 1;

    Game game;
    g_game = &game;
    game.init(hwnd);

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    LARGE_INTEGER freq, prev;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&prev);

    MSG msg = {};
    while (game.isRunning()) {
        while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                game.onClose();
                break;
            }
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }

        if (!game.isRunning()) break;

        LARGE_INTEGER now;
        QueryPerformanceCounter(&now);
        float dt = static_cast<float>(now.QuadPart - prev.QuadPart) / static_cast<float>(freq.QuadPart);
        prev = now;
        dt = std::min(dt, 0.05f);

        game.stepAndRender(dt);

        // ~60 FPS pacing
        Sleep(14);
    }

    g_game = nullptr;
    return 0;
}
