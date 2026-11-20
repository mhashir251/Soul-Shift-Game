#pragma once
#include <windows.h>

struct RectF {
    float x = 0.f;
    float y = 0.f;
    float w = 0.f;
    float h = 0.f;

    bool intersects(const RectF& other) const {
        return (x < other.x + other.w) &&
               (x + w > other.x) &&
               (y < other.y + other.h) &&
               (y + h > other.y);
    }

    RECT toWinRect() const {
        RECT r;
        r.left = static_cast<LONG>(x);
        r.top = static_cast<LONG>(y);
        r.right = static_cast<LONG>(x + w);
        r.bottom = static_cast<LONG>(y + h);
        return r;
    }
};

class GameObject {
protected:
    float x = 0.f;
    float y = 0.f;
    float width = 0.f;
    float height = 0.f;

public:
    GameObject(float px, float py, float w, float h)
        : x(px), y(py), width(w), height(h) {}
    virtual ~GameObject() = default;

    RectF getBounds() const { return {x, y, width, height}; }
    float getX() const { return x; }
    float getY() const { return y; }

    virtual void update(float dt) = 0;
    virtual void render(HDC hdc) const = 0;
};
