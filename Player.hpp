#pragma once
#include "GameObject.hpp"
#include "Obstacles.hpp"
#include <vector>
#include <memory>

class Player : public GameObject {
private:
    struct TrailGhost {
        float x;
        float y;
        float life;
    };

    float vx = 0.f;
    float vy = 0.f;
    bool isSoul = false;
    bool onGround = false;
    float soulEnergy = 100.f;
    std::vector<TrailGhost> trail;

public:
    Player(float spawnX, float spawnY);

    bool isInSoulForm() const { return isSoul; }
    float getEnergyRatio() const;

    void reset(float spawnX, float spawnY);
    bool toggleForm(const std::vector<std::unique_ptr<Obstacle>>& obstacles);
    void handleInput(float dt);
    void updatePhysics(float dt, const std::vector<std::unique_ptr<Obstacle>>& obstacles);

    void update(float dt) override;
    void render(HDC hdc) const override;
};
