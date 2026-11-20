#include "Player.hpp"
#include "Settings.hpp"
#include <algorithm>

Player::Player(float spawnX, float spawnY)
    : GameObject(spawnX, spawnY, Settings::PLAYER_WIDTH, Settings::PLAYER_HEIGHT) {}

float Player::getEnergyRatio() const {
    return std::clamp(soulEnergy / Settings::MAX_SOUL_ENERGY, 0.f, 1.f);
}

void Player::reset(float spawnX, float spawnY) {
    x = spawnX;
    y = spawnY;
    vx = 0.f;
    vy = 0.f;
    isSoul = false;
    onGround = false;
    soulEnergy = Settings::MAX_SOUL_ENERGY;
    trail.clear();
}

bool Player::toggleForm(const std::vector<std::unique_ptr<Obstacle>>& obstacles) {
    if (!isSoul) {
        if (soulEnergy >= Settings::MIN_SHIFT_ENERGY) {
            isSoul = true;
            vy *= 0.4f;
            return true;
        }
        return false;
    }

    RectF bounds = getBounds();
    for (const auto& obs : obstacles) {
        if (obs->blocksPlayer(false) && bounds.intersects(obs->getBounds())) {
            return false;
        }
    }

    isSoul = false;
    return true;
}

void Player::handleInput(float dt) {
    float speed = isSoul ? Settings::SOUL_SPEED : Settings::HUMAN_SPEED;
    vx = 0.f;

    if ((GetAsyncKeyState('A') & 0x8000) || (GetAsyncKeyState(VK_LEFT) & 0x8000)) {
        vx -= speed;
    }
    if ((GetAsyncKeyState('D') & 0x8000) || (GetAsyncKeyState(VK_RIGHT) & 0x8000)) {
        vx += speed;
    }

    bool jumpHeld = (GetAsyncKeyState('W') & 0x8000) ||
                    (GetAsyncKeyState(VK_SPACE) & 0x8000) ||
                    (GetAsyncKeyState(VK_UP) & 0x8000);

    if (!isSoul) {
        if (jumpHeld && onGround) {
            vy = Settings::HUMAN_JUMP_VEL;
            onGround = false;
        }
    } else {
        if (jumpHeld) {
            vy = std::max(vy - Settings::SOUL_FLOAT_ACCEL * dt, Settings::SOUL_MAX_RISE);
        }
    }
}

void Player::updatePhysics(float dt, const std::vector<std::unique_ptr<Obstacle>>& obstacles) {
    if (isSoul) {
        soulEnergy -= Settings::SOUL_DRAIN_RATE * dt;
        if (soulEnergy <= 0.f) {
            soulEnergy = 0.f;
            toggleForm(obstacles);
        }
    } else {
        soulEnergy = std::min(Settings::MAX_SOUL_ENERGY, soulEnergy + Settings::SOUL_RECHARGE_RATE * dt);
    }

    float gravity = isSoul ? Settings::SOUL_GRAVITY : Settings::HUMAN_GRAVITY;
    float maxFall = isSoul ? Settings::MAX_FALL_SOUL : Settings::MAX_FALL_HUMAN;
    vy = std::min(vy + gravity * dt, maxFall);

    x += vx * dt;
    RectF bounds = getBounds();
    for (const auto& obs : obstacles) {
        if (!obs->blocksPlayer(isSoul)) continue;
        RectF obsBounds = obs->getBounds();
        if (bounds.intersects(obsBounds)) {
            if (vx > 0.f) {
                x = obsBounds.x - width;
            } else if (vx < 0.f) {
                x = obsBounds.x + obsBounds.w;
            }
            bounds = getBounds();
        }
    }

    y += vy * dt;
    onGround = false;
    bounds = getBounds();
    for (const auto& obs : obstacles) {
        if (!obs->blocksPlayer(isSoul)) continue;
        RectF obsBounds = obs->getBounds();
        if (bounds.intersects(obsBounds)) {
            if (vy > 0.f) {
                y = obsBounds.y - height;
                vy = 0.f;
                onGround = true;
            } else if (vy < 0.f) {
                y = obsBounds.y + obsBounds.h;
                vy = 0.f;
            }
            bounds = getBounds();
        }
    }

    if (isSoul) {
        trail.push_back({x, y, 0.22f});
    }
    for (auto& g : trail) {
        g.life -= dt;
    }
    trail.erase(
        std::remove_if(trail.begin(), trail.end(), [](const TrailGhost& g) { return g.life <= 0.f; }),
        trail.end()
    );
}

void Player::update(float /*dt*/) {}

void Player::render(HDC hdc) const {
    if (isSoul) {
        HBRUSH ghostBrush = CreateSolidBrush(RGB(130, 75, 195));
        HPEN nullPen = CreatePen(PS_NULL, 0, 0);
        HGDIOBJ oldB = SelectObject(hdc, ghostBrush);
        HGDIOBJ oldP = SelectObject(hdc, nullPen);

        for (const auto& g : trail) {
            Rectangle(
                hdc,
                static_cast<int>(g.x),
                static_cast<int>(g.y),
                static_cast<int>(g.x + width),
                static_cast<int>(g.y + height)
            );
        }

        SelectObject(hdc, oldP);
        SelectObject(hdc, oldB);
        DeleteObject(nullPen);
        DeleteObject(ghostBrush);
    }

    COLORREF fillCol = isSoul ? Settings::SOUL_FILL : Settings::HUMAN_FILL;
    COLORREF lineCol = isSoul ? Settings::SOUL_OUTLINE : Settings::HUMAN_OUTLINE;

    HBRUSH bodyBrush = CreateSolidBrush(fillCol);
    HPEN bodyPen = CreatePen(PS_SOLID, 2, lineCol);
    HGDIOBJ oldBrush = SelectObject(hdc, bodyBrush);
    HGDIOBJ oldPen = SelectObject(hdc, bodyPen);

    RECT r = getBounds().toWinRect();
    Rectangle(hdc, r.left, r.top, r.right, r.bottom);

    SelectObject(hdc, oldPen);
    SelectObject(hdc, oldBrush);
    DeleteObject(bodyPen);
    DeleteObject(bodyBrush);
}
