#pragma once
#include "GameObject.hpp"

class Trap : public GameObject {
public:
    Trap(float px, float py, float w, float h)
        : GameObject(px, py, w, h) {}

    virtual bool isDeadlyTo(bool isSoulForm) const = 0;
};

class SpikeTrap : public Trap {
public:
    SpikeTrap(float px, float py, float w, float h);
    bool isDeadlyTo(bool isSoulForm) const override;
    void update(float dt) override;
    void render(HDC hdc) const override;
};

class AstralTrap : public Trap {
private:
    float timer = 0.f;

public:
    AstralTrap(float px, float py, float w, float h);
    bool isDeadlyTo(bool isSoulForm) const override;
    void update(float dt) override;
    void render(HDC hdc) const override;
};

class GoalPortal : public GameObject {
private:
    float animTimer = 0.f;

public:
    GoalPortal(float px, float py);
    void update(float dt) override;
    void render(HDC hdc) const override;
};
