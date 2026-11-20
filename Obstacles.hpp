#pragma once
#include "GameObject.hpp"

class Obstacle : public GameObject {
public:
    Obstacle(float px, float py, float w, float h)
        : GameObject(px, py, w, h) {}

    virtual bool blocksPlayer(bool isSoulForm) const = 0;
};

class SolidWall : public Obstacle {
public:
    SolidWall(float px, float py, float w, float h);
    bool blocksPlayer(bool isSoulForm) const override;
    void update(float dt) override;
    void render(HDC hdc) const override;
};

class SpiritBarrier : public Obstacle {
private:
    float pulseTimer = 0.f;

public:
    SpiritBarrier(float px, float py, float w, float h);
    bool blocksPlayer(bool isSoulForm) const override;
    void update(float dt) override;
    void render(HDC hdc) const override;
};
