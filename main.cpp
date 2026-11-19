// Soul Shift - single-file edition
// All project classes and implementations are consolidated in this file.
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <array>
#include <cctype>
#include <cmath>
#include <memory>
#include <string>
#include <vector>


// ===== Declarations from GameObject.hpp =====
class GameObject {
protected:
    sf::RectangleShape shape;
public:
    explicit GameObject(const sf::Vector2f& size, const sf::Vector2f& position);
    virtual ~GameObject() = default;

    virtual void update(float deltaTime) = 0;
    virtual void render(sf::RenderWindow& window) const;
    sf::FloatRect getBounds() const;
};

// ===== Declarations from Character.hpp =====
class Character : public GameObject {
protected:
    float speed;
    sf::CircleShape head;
    sf::CircleShape eye;
    sf::RectangleShape torso;
    sf::RectangleShape leftArm;
    sf::RectangleShape rightArm;
    sf::RectangleShape leftLeg;
    sf::RectangleShape rightLeg;
    float verticalVelocity;
    float animationTime;
    float movementDirection;
    bool grounded;
    bool facingLeft;

public:
    Character(const sf::Vector2f& size, const sf::Vector2f& position, float movementSpeed,
              const sf::Color& skinColor, const sf::Color& bodyColor,
              const sf::Color& limbColor, const sf::Color& detailColor);
    ~Character() override = default;

    virtual bool canPassMagicBarrier() const = 0;
    virtual bool canEnterWater() const = 0;
    virtual bool canFloatAcrossGap() const = 0;
    virtual bool canFly() const = 0;
    virtual bool canPushObjects() const = 0;
    virtual bool canActivateSwitch() const = 0;

    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) const override;
    void move(float dx, float dy);
    void setMovementDirection(float direction);
    float getHorizontalVelocity() const;
    float getMovementSpeed() const;
    float getVerticalVelocity() const;
    void setVerticalVelocity(float velocity);
    void applyGravity(float deltaTime);
    void bounceFromTrampoline();
    bool jump();
    void setGrounded(bool isGrounded);
    bool isGrounded() const;
    void stopVerticalMovement();
};

// ===== Declarations from Human.hpp =====
class Human : public Character {
public:
    explicit Human(const sf::Vector2f& position);
    bool canPassMagicBarrier() const override;
    bool canEnterWater() const override;
    bool canFloatAcrossGap() const override;
    bool canFly() const override;
    bool canPushObjects() const override;
    bool canActivateSwitch() const override;
};

// ===== Declarations from Ghost.hpp =====
class Ghost : public Character {
public:
    explicit Ghost(const sf::Vector2f& position);
    void render(sf::RenderWindow& window) const override;
    bool canPassMagicBarrier() const override;
    bool canEnterWater() const override;
    bool canFloatAcrossGap() const override;
    bool canFly() const override;
    bool canPushObjects() const override;
    bool canActivateSwitch() const override;
};

// ===== Declarations from Obstacle.hpp =====
enum class ObstacleType { Wall, Water, MagicBarrier, Gap, Trampoline, Platform };

class Obstacle : public GameObject {
    ObstacleType type;
    float animationTime;
public:
    Obstacle(ObstacleType obstacleType, const sf::Vector2f& size,
             const sf::Vector2f& position);
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) const override;
    bool blocks(const Character& character) const;
    bool isTrampoline() const;
};

// ===== Declarations from Switch.hpp =====
class Switch : public GameObject {
    bool pressed;
public:
    explicit Switch(const sf::Vector2f& position);
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) const override;
    void setPressed(bool value);
};

// ===== Declarations from Door.hpp =====
class Door : public GameObject {
    bool open;
public:
    explicit Door(const sf::Vector2f& position);
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) const override;
    void setOpen(bool value);
    bool isOpen() const;
    bool blocks() const;
};

// ===== Declarations from Game.hpp =====
class Game {
    enum class Screen {
        MainMenu,
        LevelSelect,
        HowToPlay,
        Settings,
        ConfirmExit,
        Playing,
        Paused,
        LevelComplete
    };

    sf::RenderWindow window;
    std::unique_ptr<Character> player;
    std::vector<std::unique_ptr<Obstacle>> obstacles;
    std::vector<std::unique_ptr<Switch>> pressurePlates;
    std::vector<std::unique_ptr<Door>> doors;
    Screen screen;
    int currentLevel;
    int selectedOption;
    bool isHuman;
    bool fullscreen;
    float doorCloseTimer;
    float visualTime;
    void processEvents();
    void update(float deltaTime);
    void render();
    void switchForm();
    void loadLevel(int level);
    void startLevel(int level);
    void activateSelection();
    void handleMenuClick(const sf::Vector2f& position);
    void toggleFullscreen();
    void renderMenu();
    void renderHud();
public:
    Game();
    void run();
};


// ===== Implementation from GameObject.cpp =====
GameObject::GameObject(const sf::Vector2f& size, const sf::Vector2f& position) {
    shape.setSize(size);
    shape.setPosition(position);
}

void GameObject::render(sf::RenderWindow& window) const {
    window.draw(shape);
}

sf::FloatRect GameObject::getBounds() const {
    return shape.getGlobalBounds();
}


// ===== Implementation from Character.cpp =====
Character::Character(const sf::Vector2f& size, const sf::Vector2f& position,
                     float movementSpeed, const sf::Color& skinColor,
                     const sf::Color& bodyColor, const sf::Color& limbColor,
                     const sf::Color& detailColor)
    : GameObject(size, position),
      speed(movementSpeed),
      head(9.f),
      eye(1.5f),
      torso(sf::Vector2f(18.f, 20.f)),
      leftArm(sf::Vector2f(6.f, 17.f)),
      rightArm(sf::Vector2f(6.f, 17.f)),
      leftLeg(sf::Vector2f(7.f, 15.f)),
      rightLeg(sf::Vector2f(7.f, 15.f)),
      verticalVelocity(0.f),
      animationTime(0.f),
      movementDirection(0.f),
      grounded(false),
      facingLeft(false) {
    head.setFillColor(skinColor);
    head.setOutlineColor(detailColor);
    head.setOutlineThickness(1.f);
    eye.setFillColor(detailColor);
    torso.setFillColor(bodyColor);
    leftArm.setFillColor(bodyColor);
    rightArm.setFillColor(bodyColor);
    leftLeg.setFillColor(limbColor);
    rightLeg.setFillColor(limbColor);
}

void Character::update(float deltaTime) {
    if (movementDirection != 0.f && grounded) {
        animationTime += deltaTime;
    } else {
        animationTime = 0.f;
    }
}

void Character::render(sf::RenderWindow& window) const {
    const sf::Vector2f position = shape.getPosition();
    const float legSwing = movementDirection != 0.f && grounded
        ? std::sin(animationTime * 14.f) * 3.f
        : 0.f;
    const float armSwing = -legSwing;
    sf::RectangleShape renderedLeftLeg(leftLeg);
    sf::RectangleShape renderedRightLeg(rightLeg);
    sf::RectangleShape renderedLeftArm(leftArm);
    sf::RectangleShape renderedRightArm(rightArm);
    sf::RectangleShape renderedTorso(torso);
    sf::CircleShape renderedHead(head);
    sf::CircleShape renderedEye(eye);

    renderedLeftLeg.setPosition(position.x + 8.f + legSwing, position.y + 32.f);
    renderedRightLeg.setPosition(position.x + 17.f - legSwing, position.y + 32.f);
    renderedLeftArm.setPosition(position.x + 1.f, position.y + 18.f + armSwing);
    renderedRightArm.setPosition(position.x + 25.f, position.y + 18.f - armSwing);
    renderedTorso.setPosition(position.x + 7.f, position.y + 16.f);
    renderedHead.setPosition(position.x + 7.f, position.y);

    window.draw(renderedLeftLeg);
    window.draw(renderedRightLeg);
    window.draw(renderedLeftArm);
    window.draw(renderedRightArm);
    window.draw(renderedTorso);
    window.draw(renderedHead);

    const float eyeX = facingLeft ? position.x + 9.f : position.x + 18.f;
    renderedEye.setPosition(eyeX, position.y + 7.f);
    window.draw(renderedEye);
}

void Character::move(float dx, float dy) { shape.move(dx, dy); }

void Character::setMovementDirection(float direction) {
    movementDirection = std::max(-1.f, std::min(direction, 1.f));
    if (movementDirection != 0.f) {
        facingLeft = movementDirection < 0.f;
    }
}

float Character::getHorizontalVelocity() const {
    return movementDirection * speed;
}

float Character::getMovementSpeed() const {
    return speed;
}

float Character::getVerticalVelocity() const {
    return verticalVelocity;
}

void Character::setVerticalVelocity(float velocity) {
    verticalVelocity = velocity;
}

void Character::applyGravity(float deltaTime) {
    if (!grounded) {
        verticalVelocity = std::min(verticalVelocity + 1000.f * deltaTime, 700.f);
    }
}

void Character::bounceFromTrampoline() {
    verticalVelocity = -680.f;
    grounded = false;
}

bool Character::jump() {
    if (canFly() || !grounded) {
        return false;
    }

    verticalVelocity = -440.f;
    grounded = false;
    return true;
}

void Character::setGrounded(bool isGrounded) {
    grounded = isGrounded;
    if (grounded && verticalVelocity > 0.f) {
        verticalVelocity = 0.f;
    }
}

bool Character::isGrounded() const {
    return grounded;
}

void Character::stopVerticalMovement() {
    verticalVelocity = 0.f;
}


// ===== Implementation from Human.cpp =====
Human::Human(const sf::Vector2f& position)
    : Character(sf::Vector2f(32.f, 48.f), position, 220.f,
                sf::Color(247, 194, 139), sf::Color(190, 99, 55),
                sf::Color(112, 65, 47), sf::Color(43, 30, 42)) {}

bool Human::canPassMagicBarrier() const { return false; }
bool Human::canEnterWater() const { return false; }
bool Human::canFloatAcrossGap() const { return false; }
bool Human::canFly() const { return false; }
bool Human::canPushObjects() const { return true; }
bool Human::canActivateSwitch() const { return true; }


// ===== Implementation from Ghost.cpp =====
Ghost::Ghost(const sf::Vector2f& position)
    : Character(sf::Vector2f(32.f, 48.f), position, 220.f,
                sf::Color(175, 215, 220, 225), sf::Color(70, 100, 145, 190),
                sf::Color(45, 70, 115, 190), sf::Color(30, 25, 45)) {}

void Ghost::render(sf::RenderWindow& window) const {
    const sf::FloatRect bounds = getBounds();
    sf::CircleShape aura(27.f);
    aura.setScale(0.7f, 1.f);
    aura.setPosition(bounds.left - 4.f, bounds.top - 2.f);
    aura.setFillColor(sf::Color(65, 205, 239, 34));
    window.draw(aura);

    Character::render(window);

    sf::CircleShape eye(3.f);
    eye.setFillColor(sf::Color(105, 235, 255));
    eye.setOutlineColor(sf::Color(13, 69, 103));
    eye.setOutlineThickness(1.f);
    eye.setPosition(bounds.left + 8.f, bounds.top + 6.f);
    window.draw(eye);
    eye.setPosition(bounds.left + 18.f, bounds.top + 6.f);
    window.draw(eye);

    sf::ConvexShape mouth(4);
    mouth.setPoint(0, sf::Vector2f(0.f, 0.f));
    mouth.setPoint(1, sf::Vector2f(14.f, 0.f));
    mouth.setPoint(2, sf::Vector2f(12.f, 8.f));
    mouth.setPoint(3, sf::Vector2f(2.f, 8.f));
    mouth.setPosition(bounds.left + 9.f, bounds.top + 24.f);
    mouth.setFillColor(sf::Color(28, 12, 30));
    window.draw(mouth);

    sf::ConvexShape fang(3);
    fang.setPoint(0, sf::Vector2f(0.f, 0.f));
    fang.setPoint(1, sf::Vector2f(5.f, 0.f));
    fang.setPoint(2, sf::Vector2f(2.5f, 5.f));
    fang.setPosition(bounds.left + 10.f, bounds.top + 24.f);
    fang.setFillColor(sf::Color(245, 245, 225));
    window.draw(fang);
    fang.setPosition(bounds.left + 18.f, bounds.top + 24.f);
    window.draw(fang);
}

bool Ghost::canPassMagicBarrier() const { return true; }
bool Ghost::canEnterWater() const { return false; }
bool Ghost::canFloatAcrossGap() const { return true; }
bool Ghost::canFly() const { return true; }
bool Ghost::canPushObjects() const { return false; }
bool Ghost::canActivateSwitch() const { return false; }


// ===== Implementation from Obstacle.cpp =====
Obstacle::Obstacle(ObstacleType obstacleType, const sf::Vector2f& size,
                   const sf::Vector2f& position)
    : GameObject(size, position), type(obstacleType), animationTime(0.f) {
    switch (type) {
        case ObstacleType::Wall: shape.setFillColor(sf::Color(95, 100, 125)); break;
        case ObstacleType::Water: shape.setFillColor(sf::Color(35, 125, 220)); break;
        case ObstacleType::MagicBarrier: shape.setFillColor(sf::Color(180, 70, 230)); break;
        case ObstacleType::Gap: shape.setFillColor(sf::Color::Black); break;
        case ObstacleType::Trampoline: shape.setFillColor(sf::Color(235, 120, 185)); break;
        case ObstacleType::Platform: shape.setFillColor(sf::Color(100, 67, 45)); break;
    }
}
void Obstacle::update(float deltaTime) {
    animationTime += deltaTime;
}

void Obstacle::render(sf::RenderWindow& window) const {
    if (type == ObstacleType::Platform) {
        GameObject::render(window);
        const sf::FloatRect bounds = getBounds();
        sf::RectangleShape edge(sf::Vector2f(bounds.width, 2.f));
        edge.setPosition(bounds.left, bounds.top + bounds.height - 2.f);
        edge.setFillColor(sf::Color(57, 39, 43));
        window.draw(edge);

        sf::RectangleShape grass(sf::Vector2f(bounds.width, 6.f));
        grass.setPosition(bounds.left, bounds.top);
        grass.setFillColor(sf::Color(71, 177, 115));
        window.draw(grass);

        sf::RectangleShape seam(sf::Vector2f(2.f, 5.f));
        seam.setFillColor(sf::Color(76, 48, 47));
        for (float x = bounds.left + 18.f; x < bounds.left + bounds.width; x += 32.f) {
            seam.setPosition(x, bounds.top + 8.f);
            window.draw(seam);
        }
        return;
    }

    if (type == ObstacleType::Water) {
        const sf::FloatRect bounds = getBounds();
        sf::RectangleShape water(sf::Vector2f(bounds.width, bounds.height));
        water.setPosition(bounds.left, bounds.top);
        water.setFillColor(sf::Color(24, 93, 154, 225));
        window.draw(water);

        sf::RectangleShape surface(sf::Vector2f(bounds.width, 3.f));
        surface.setPosition(bounds.left, bounds.top);
        surface.setFillColor(sf::Color(82, 199, 222, 220));
        window.draw(surface);

        for (int row = 0; row < 2; ++row) {
            sf::VertexArray wave(sf::LineStrip, 19);
            const float y = bounds.top + 7.f + row * 9.f;
            const float phase = animationTime * (2.4f + row * 0.5f) + row * 1.7f;
            const sf::Color color = row == 0
                ? sf::Color(155, 238, 239, 210)
                : sf::Color(69, 164, 201, 190);
            for (std::size_t i = 0; i < wave.getVertexCount(); ++i) {
                const float x = bounds.left + bounds.width * static_cast<float>(i) /
                    static_cast<float>(wave.getVertexCount() - 1);
                wave[i].position = sf::Vector2f(
                    x, y + std::sin(x * 0.055f + phase) * 2.f);
                wave[i].color = color;
            }
            window.draw(wave);
        }
        return;
    }

    if (type == ObstacleType::MagicBarrier) {
        const sf::FloatRect bounds = getBounds();
        const float pulse = 0.5f + 0.5f * std::sin(animationTime * 3.f);
        sf::RectangleShape glow(sf::Vector2f(bounds.width + 22.f, bounds.height + 16.f));
        glow.setPosition(bounds.left - 11.f, bounds.top - 8.f);
        glow.setFillColor(sf::Color(118, 47, 202,
            static_cast<sf::Uint8>(28.f + pulse * 34.f)));
        window.draw(glow);

        sf::RectangleShape field(sf::Vector2f(bounds.width, bounds.height));
        field.setPosition(bounds.left, bounds.top);
        field.setFillColor(sf::Color(88, 42, 165, 165));
        field.setOutlineColor(sf::Color(207, 143, 255, 190));
        field.setOutlineThickness(1.f);
        window.draw(field);

        sf::RectangleShape core(sf::Vector2f(4.f, bounds.height - 10.f));
        core.setPosition(bounds.left + (bounds.width - core.getSize().x) / 2.f,
                         bounds.top + 5.f);
        core.setFillColor(sf::Color(229, 193, 255,
            static_cast<sf::Uint8>(130.f + pulse * 100.f)));
        window.draw(core);

        sf::CircleShape rune(3.f);
        rune.setOrigin(3.f, 3.f);
        rune.setFillColor(sf::Color(245, 220, 255,
            static_cast<sf::Uint8>(150.f + pulse * 100.f)));
        for (float y = bounds.top + 18.f; y < bounds.top + bounds.height; y += 38.f) {
            rune.setPosition(bounds.left + bounds.width / 2.f, y);
            window.draw(rune);
        }
        return;
    }

    if (type != ObstacleType::Trampoline) {
        GameObject::render(window);
        return;
    }

    const sf::FloatRect bounds = getBounds();
    sf::RectangleShape base(sf::Vector2f(bounds.width - 12.f, 5.f));
    base.setPosition(bounds.left + 6.f, bounds.top + 13.f);
    base.setFillColor(sf::Color(80, 48, 96));
    window.draw(base);

    sf::RectangleShape spring(sf::Vector2f(3.f, 9.f));
    spring.setFillColor(sf::Color(210, 210, 225));
    for (int i = 0; i < 5; ++i) {
        spring.setPosition(bounds.left + 12.f + i * 11.f, bounds.top + 5.f);
        window.draw(spring);
    }

    sf::RectangleShape mat(sf::Vector2f(bounds.width, 6.f));
    mat.setPosition(bounds.left, bounds.top);
    mat.setFillColor(sf::Color(245, 125, 185));
    window.draw(mat);
}

bool Obstacle::blocks(const Character& character) const {
    if (type == ObstacleType::Wall || type == ObstacleType::Platform) return true;
    if (type == ObstacleType::Water) return !character.canEnterWater();
    if (type == ObstacleType::MagicBarrier) return !character.canPassMagicBarrier();
    return false;
}

bool Obstacle::isTrampoline() const {
    return type == ObstacleType::Trampoline;
}


// ===== Implementation from Switch.cpp =====
Switch::Switch(const sf::Vector2f& position)
    : GameObject(sf::Vector2f(52.f, 12.f), position), pressed(false) {
    shape.setFillColor(sf::Color(240, 150, 45));
}
void Switch::update(float) {}

void Switch::render(sf::RenderWindow& window) const {
    const sf::FloatRect bounds = getBounds();
    sf::RectangleShape base(sf::Vector2f(bounds.width, bounds.height));
    base.setPosition(bounds.left, bounds.top);
    base.setFillColor(sf::Color(48, 43, 62));
    base.setOutlineColor(sf::Color(126, 111, 153));
    base.setOutlineThickness(1.f);
    window.draw(base);

    sf::RectangleShape plate(sf::Vector2f(bounds.width - 10.f,
        pressed ? 4.f : 7.f));
    plate.setOrigin(0.f, plate.getSize().y / 2.f);
    plate.setPosition(bounds.left + 5.f, bounds.top + bounds.height / 2.f);
    plate.setFillColor(pressed ? sf::Color(114, 255, 169) : sf::Color(255, 187, 95));
    window.draw(plate);

    sf::CircleShape indicator(2.f);
    indicator.setFillColor(pressed ? sf::Color(185, 255, 211) : sf::Color(255, 223, 157));
    indicator.setPosition(bounds.left + bounds.width - 7.f, bounds.top + 4.f);
    window.draw(indicator);
}

void Switch::setPressed(bool value) {
    pressed = value;
    shape.setFillColor(pressed ? sf::Color(70, 220, 100) : sf::Color(240, 150, 45));
}


// ===== Implementation from Door.cpp =====
Door::Door(const sf::Vector2f& position)
    : GameObject(sf::Vector2f(42.f, 180.f), position), open(false) {
    shape.setFillColor(sf::Color(150, 75, 35));
}
void Door::update(float) {}

void Door::render(sf::RenderWindow& window) const {
    const sf::FloatRect bounds = getBounds();
    if (!open) {
        GameObject::render(window);

        sf::RectangleShape inset(sf::Vector2f(bounds.width - 12.f, bounds.height - 10.f));
        inset.setPosition(bounds.left + 6.f, bounds.top + 5.f);
        inset.setFillColor(sf::Color(113, 55, 54));
        inset.setOutlineColor(sf::Color(202, 139, 90));
        inset.setOutlineThickness(1.f);
        window.draw(inset);

        sf::RectangleShape panelLine(sf::Vector2f(2.f, bounds.height - 24.f));
        panelLine.setPosition(bounds.left + bounds.width / 2.f - 1.f, bounds.top + 12.f);
        panelLine.setFillColor(sf::Color(78, 40, 48));
        window.draw(panelLine);

        sf::CircleShape handle(2.f);
        handle.setFillColor(sf::Color(245, 205, 90));
        handle.setPosition(bounds.left + bounds.width - 9.f, bounds.top + 48.f);
        window.draw(handle);
        return;
    }

    sf::RectangleShape frame(sf::Vector2f(5.f, bounds.height));
    frame.setFillColor(sf::Color(110, 75, 45));
    frame.setPosition(bounds.left, bounds.top);
    window.draw(frame);
    frame.setPosition(bounds.left + bounds.width - 5.f, bounds.top);
    window.draw(frame);

    sf::RectangleShape lintel(sf::Vector2f(bounds.width, 5.f));
    lintel.setFillColor(sf::Color(110, 75, 45));
    lintel.setPosition(bounds.left, bounds.top);
    window.draw(lintel);

    sf::RectangleShape openPanel(sf::Vector2f(9.f, bounds.height - 8.f));
    openPanel.setFillColor(sf::Color(70, 180, 95));
    openPanel.setOutlineColor(sf::Color(155, 240, 174));
    openPanel.setOutlineThickness(1.f);
    openPanel.setPosition(bounds.left + bounds.width - 14.f, bounds.top + 5.f);
    window.draw(openPanel);
}

void Door::setOpen(bool value) {
    open = value;
    shape.setFillColor(open ? sf::Color(70, 210, 100) : sf::Color(150, 75, 35));
}
bool Door::isOpen() const { return open; }
bool Door::blocks() const { return !open; }


// ===== Implementation from Game.cpp =====
namespace {
std::array<const char*, 7> glyphFor(char character) {
    switch (static_cast<char>(std::toupper(static_cast<unsigned char>(character)))) {
        case 'A': return {{"01110", "10001", "10001", "11111", "10001", "10001", "10001"}};
        case 'B': return {{"11110", "10001", "10001", "11110", "10001", "10001", "11110"}};
        case 'C': return {{"01111", "10000", "10000", "10000", "10000", "10000", "01111"}};
        case 'D': return {{"11110", "10001", "10001", "10001", "10001", "10001", "11110"}};
        case 'E': return {{"11111", "10000", "10000", "11110", "10000", "10000", "11111"}};
        case 'F': return {{"11111", "10000", "10000", "11110", "10000", "10000", "10000"}};
        case 'G': return {{"01111", "10000", "10000", "10111", "10001", "10001", "01111"}};
        case 'H': return {{"10001", "10001", "10001", "11111", "10001", "10001", "10001"}};
        case 'I': return {{"11111", "00100", "00100", "00100", "00100", "00100", "11111"}};
        case 'J': return {{"00111", "00010", "00010", "00010", "10010", "10010", "01100"}};
        case 'K': return {{"10001", "10010", "10100", "11000", "10100", "10010", "10001"}};
        case 'L': return {{"10000", "10000", "10000", "10000", "10000", "10000", "11111"}};
        case 'M': return {{"10001", "11011", "10101", "10101", "10001", "10001", "10001"}};
        case 'N': return {{"10001", "11001", "10101", "10011", "10001", "10001", "10001"}};
        case 'O': return {{"01110", "10001", "10001", "10001", "10001", "10001", "01110"}};
        case 'P': return {{"11110", "10001", "10001", "11110", "10000", "10000", "10000"}};
        case 'Q': return {{"01110", "10001", "10001", "10001", "10101", "10010", "01101"}};
        case 'R': return {{"11110", "10001", "10001", "11110", "10100", "10010", "10001"}};
        case 'S': return {{"01111", "10000", "10000", "01110", "00001", "00001", "11110"}};
        case 'T': return {{"11111", "00100", "00100", "00100", "00100", "00100", "00100"}};
        case 'U': return {{"10001", "10001", "10001", "10001", "10001", "10001", "01110"}};
        case 'V': return {{"10001", "10001", "10001", "10001", "10001", "01010", "00100"}};
        case 'W': return {{"10001", "10001", "10001", "10101", "10101", "10101", "01010"}};
        case 'X': return {{"10001", "10001", "01010", "00100", "01010", "10001", "10001"}};
        case 'Y': return {{"10001", "10001", "01010", "00100", "00100", "00100", "00100"}};
        case 'Z': return {{"11111", "00001", "00010", "00100", "01000", "10000", "11111"}};
        case '0': return {{"01110", "10001", "10011", "10101", "11001", "10001", "01110"}};
        case '1': return {{"00100", "01100", "00100", "00100", "00100", "00100", "01110"}};
        case '2': return {{"01110", "10001", "00001", "00010", "00100", "01000", "11111"}};
        case '3': return {{"11110", "00001", "00001", "01110", "00001", "00001", "11110"}};
        case '4': return {{"00010", "00110", "01010", "10010", "11111", "00010", "00010"}};
        case '5': return {{"11111", "10000", "10000", "11110", "00001", "00001", "11110"}};
        case '6': return {{"01110", "10000", "10000", "11110", "10001", "10001", "01110"}};
        case '7': return {{"11111", "00001", "00010", "00100", "01000", "01000", "01000"}};
        case '8': return {{"01110", "10001", "10001", "01110", "10001", "10001", "01110"}};
        case '9': return {{"01110", "10001", "10001", "01111", "00001", "00001", "01110"}};
        case ':': return {{"00000", "00100", "00100", "00000", "00100", "00100", "00000"}};
        case '.': return {{"00000", "00000", "00000", "00000", "00000", "00110", "00110"}};
        case '-': return {{"00000", "00000", "00000", "11111", "00000", "00000", "00000"}};
        case '/': return {{"00001", "00010", "00010", "00100", "01000", "01000", "10000"}};
        case '?': return {{"01110", "10001", "00001", "00010", "00100", "00000", "00100"}};
        default: return {{"00000", "00000", "00000", "00000", "00000", "00000", "00000"}};
    }
}

void drawPixelText(sf::RenderWindow& window, const std::string& text,
                   float x, float y, float scale, const sf::Color& color,
                   bool centered = false) {
    if (centered) {
        x -= (static_cast<float>(text.size()) * 6.f - 1.f) * scale / 2.f;
    }

    sf::VertexArray pixels(sf::Quads);
    for (std::size_t characterIndex = 0; characterIndex < text.size(); ++characterIndex) {
        const auto glyph = glyphFor(text[characterIndex]);
        for (std::size_t row = 0; row < glyph.size(); ++row) {
            for (std::size_t column = 0; column < 5; ++column) {
                if (glyph[row][column] != '1') {
                    continue;
                }

                const float left = x + (static_cast<float>(characterIndex) * 6.f +
                    static_cast<float>(column)) * scale;
                const float top = y + static_cast<float>(row) * scale;
                pixels.append(sf::Vertex(sf::Vector2f(left, top), color));
                pixels.append(sf::Vertex(sf::Vector2f(left + scale, top), color));
                pixels.append(sf::Vertex(sf::Vector2f(left + scale, top + scale), color));
                pixels.append(sf::Vertex(sf::Vector2f(left, top + scale), color));
            }
        }
    }
    window.draw(pixels);
}

sf::FloatRect buttonBounds(const sf::Vector2u& size, int index, int count, float scale) {
    const float width = 300.f * scale;
    const float height = 42.f * scale;
    const float gap = 10.f * scale;
    const float totalHeight = static_cast<float>(count) * height +
        static_cast<float>(count - 1) * gap;
    const float top = (static_cast<float>(size.y) - totalHeight) / 2.f + 48.f * scale;
    return sf::FloatRect((static_cast<float>(size.x) - width) / 2.f,
        top + static_cast<float>(index) * (height + gap), width, height);
}

void drawButton(sf::RenderWindow& window, const sf::FloatRect& bounds,
                const std::string& label, float scale, bool selected) {
    sf::RectangleShape shadow(sf::Vector2f(bounds.width, bounds.height));
    shadow.setPosition(bounds.left, bounds.top + 4.f * scale);
    shadow.setFillColor(sf::Color(5, 8, 18, 180));
    window.draw(shadow);

    sf::RectangleShape button(sf::Vector2f(bounds.width, bounds.height));
    button.setPosition(bounds.left, bounds.top);
    button.setFillColor(selected ? sf::Color(49, 56, 83, 245)
                                 : sf::Color(24, 30, 50, 230));
    button.setOutlineColor(selected ? sf::Color(119, 207, 226)
                                    : sf::Color(77, 93, 122));
    button.setOutlineThickness(selected ? 2.f * scale : scale);
    window.draw(button);

    drawPixelText(window, label, bounds.left + bounds.width / 2.f,
        bounds.top + (bounds.height - 7.f * 2.2f * scale) / 2.f,
        2.2f * scale, selected ? sf::Color(227, 247, 255)
                               : sf::Color(184, 197, 220), true);
}

void drawBackdrop(sf::RenderWindow& window, float time) {
    const sf::Vector2u size = window.getSize();
    sf::RectangleShape band(sf::Vector2f(static_cast<float>(size.x), 30.f));
    for (unsigned int y = 0; y < size.y; y += 30) {
        const float blend = static_cast<float>(y) / static_cast<float>(size.y);
        band.setPosition(0.f, static_cast<float>(y));
        band.setFillColor(sf::Color(
            static_cast<sf::Uint8>(12.f + blend * 19.f),
            static_cast<sf::Uint8>(16.f + blend * 18.f),
            static_cast<sf::Uint8>(35.f + blend * 39.f)));
        window.draw(band);
    }

    sf::CircleShape moon(46.f, 48);
    moon.setOrigin(46.f, 46.f);
    moon.setPosition(static_cast<float>(size.x) * 0.78f,
                     static_cast<float>(size.y) * 0.24f);
    moon.setFillColor(sf::Color(123, 177, 212, 25));
    window.draw(moon);
    moon.setRadius(30.f);
    moon.setOrigin(30.f, 30.f);
    moon.setFillColor(sf::Color(190, 218, 228, 185));
    window.draw(moon);

    sf::CircleShape star(1.5f);
    for (unsigned int i = 0; i < 48; ++i) {
        const float x = static_cast<float>((i * 137u + 53u) % size.x);
        const float y = static_cast<float>((i * 71u + 29u) % (size.y * 3u / 4u));
        const sf::Uint8 alpha = static_cast<sf::Uint8>(
            90u + (i % 5u) * 30u + static_cast<unsigned int>(
                (std::sin(time * 1.4f + static_cast<float>(i)) + 1.f) * 18.f));
        star.setFillColor(sf::Color(174, 215, 243, alpha));
        star.setPosition(x, y);
        window.draw(star);
    }

    sf::ConvexShape ridge(4);
    ridge.setPoint(0, sf::Vector2f(0.f, 90.f));
    ridge.setPoint(1, sf::Vector2f(static_cast<float>(size.x) * 0.32f, 0.f));
    ridge.setPoint(2, sf::Vector2f(static_cast<float>(size.x) * 0.7f, 100.f));
    ridge.setPoint(3, sf::Vector2f(static_cast<float>(size.x), 28.f));
    ridge.setPosition(0.f, static_cast<float>(size.y) * 0.66f);
    ridge.setFillColor(sf::Color(21, 31, 51));
    window.draw(ridge);
}
}

Game::Game()
    : window(sf::VideoMode(900, 540), "Soul Shift"),
      screen(Screen::MainMenu),
      currentLevel(1),
      selectedOption(0),
      isHuman(true),
      fullscreen(false),
      doorCloseTimer(0.f),
      visualTime(0.f) {
    window.setFramerateLimit(60);
    window.setKeyRepeatEnabled(false);
    loadLevel(1);
    screen = Screen::MainMenu;
}

void Game::loadLevel(int level) {
    currentLevel = level;
    obstacles.clear();
    pressurePlates.clear();
    doors.clear();
    doorCloseTimer = 0.f;
    isHuman = true;
    player = std::make_unique<Human>(sf::Vector2f(80.f, 390.f));

    obstacles.push_back(std::make_unique<Obstacle>(
        ObstacleType::Platform, sf::Vector2f(900.f, 40.f), sf::Vector2f(0.f, 500.f)));
    obstacles.push_back(std::make_unique<Obstacle>(
        ObstacleType::Wall, sf::Vector2f(40.f, 540.f), sf::Vector2f(0.f, 0.f)));
    obstacles.push_back(std::make_unique<Obstacle>(
        ObstacleType::Wall, sf::Vector2f(40.f, 540.f), sf::Vector2f(860.f, 0.f)));

    if (level == 1) {
        obstacles.push_back(std::make_unique<Obstacle>(
            ObstacleType::Platform, sf::Vector2f(105.f, 18.f), sf::Vector2f(145.f, 430.f)));
        obstacles.push_back(std::make_unique<Obstacle>(
            ObstacleType::Platform, sf::Vector2f(105.f, 18.f), sf::Vector2f(290.f, 375.f)));
        obstacles.push_back(std::make_unique<Obstacle>(
            ObstacleType::Platform, sf::Vector2f(105.f, 18.f), sf::Vector2f(420.f, 325.f)));
        obstacles.push_back(std::make_unique<Obstacle>(
            ObstacleType::Trampoline, sf::Vector2f(68.f, 18.f), sf::Vector2f(477.f, 482.f)));
        obstacles.push_back(std::make_unique<Obstacle>(
            ObstacleType::Water, sf::Vector2f(150.f, 25.f), sf::Vector2f(300.f, 475.f)));
        obstacles.push_back(std::make_unique<Obstacle>(
            ObstacleType::MagicBarrier, sf::Vector2f(28.f, 180.f), sf::Vector2f(560.f, 320.f)));
        pressurePlates.push_back(std::make_unique<Switch>(sf::Vector2f(635.f, 488.f)));
        doors.push_back(std::make_unique<Door>(sf::Vector2f(700.f, 320.f)));
    } else {
        obstacles.push_back(std::make_unique<Obstacle>(
            ObstacleType::Platform, sf::Vector2f(120.f, 18.f), sf::Vector2f(105.f, 440.f)));
        obstacles.push_back(std::make_unique<Obstacle>(
            ObstacleType::Platform, sf::Vector2f(110.f, 18.f), sf::Vector2f(270.f, 370.f)));
        obstacles.push_back(std::make_unique<Obstacle>(
            ObstacleType::Platform, sf::Vector2f(100.f, 18.f), sf::Vector2f(405.f, 420.f)));
        obstacles.push_back(std::make_unique<Obstacle>(
            ObstacleType::Platform, sf::Vector2f(115.f, 18.f), sf::Vector2f(535.f, 345.f)));
        obstacles.push_back(std::make_unique<Obstacle>(
            ObstacleType::Platform, sf::Vector2f(105.f, 18.f), sf::Vector2f(675.f, 405.f)));
        obstacles.push_back(std::make_unique<Obstacle>(
            ObstacleType::Trampoline, sf::Vector2f(68.f, 18.f), sf::Vector2f(360.f, 482.f)));
        obstacles.push_back(std::make_unique<Obstacle>(
            ObstacleType::Water, sf::Vector2f(135.f, 25.f), sf::Vector2f(230.f, 475.f)));
        obstacles.push_back(std::make_unique<Obstacle>(
            ObstacleType::MagicBarrier, sf::Vector2f(28.f, 200.f), sf::Vector2f(505.f, 300.f)));
        pressurePlates.push_back(std::make_unique<Switch>(sf::Vector2f(625.f, 488.f)));
        doors.push_back(std::make_unique<Door>(sf::Vector2f(740.f, 320.f)));
    }
}

void Game::startLevel(int level) {
    loadLevel(level);
    screen = Screen::Playing;
    selectedOption = 0;
}

void Game::toggleFullscreen() {
    fullscreen = !fullscreen;
    if (fullscreen) {
        window.create(sf::VideoMode::getDesktopMode(), "Soul Shift", sf::Style::Fullscreen);
    } else {
        window.create(sf::VideoMode(900, 540), "Soul Shift", sf::Style::Titlebar |
                      sf::Style::Close);
    }
    window.setFramerateLimit(60);
    window.setKeyRepeatEnabled(false);
}

void Game::processEvents() {
    sf::Event event{};
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
            continue;
        }

        if (event.type == sf::Event::MouseButtonPressed &&
            event.mouseButton.button == sf::Mouse::Left) {
            handleMenuClick(sf::Vector2f(static_cast<float>(event.mouseButton.x),
                                         static_cast<float>(event.mouseButton.y)));
            continue;
        }

        if (event.type != sf::Event::KeyPressed) {
            continue;
        }

        if (screen == Screen::Playing) {
            if (event.key.code == sf::Keyboard::Escape) {
                screen = Screen::Paused;
                selectedOption = 0;
            } else if (event.key.code == sf::Keyboard::Q ||
                       event.key.code == sf::Keyboard::G) {
                switchForm();
            } else if (event.key.code == sf::Keyboard::Space) {
                player->jump();
            } else if (event.key.code == sf::Keyboard::F) {
                toggleFullscreen();
            }
            continue;
        }

        if (event.key.code == sf::Keyboard::Escape) {
            switch (screen) {
                case Screen::MainMenu:
                    screen = Screen::ConfirmExit;
                    selectedOption = 1;
                    break;
                case Screen::ConfirmExit:
                    screen = Screen::MainMenu;
                    selectedOption = 0;
                    break;
                case Screen::Paused:
                    screen = Screen::Playing;
                    break;
                case Screen::LevelComplete:
                case Screen::LevelSelect:
                case Screen::HowToPlay:
                case Screen::Settings:
                    screen = Screen::MainMenu;
                    selectedOption = 0;
                    break;
                case Screen::Playing:
                    break;
            }
            continue;
        }

        if (event.key.code == sf::Keyboard::Up || event.key.code == sf::Keyboard::W) {
            --selectedOption;
        } else if (event.key.code == sf::Keyboard::Down ||
                   event.key.code == sf::Keyboard::S) {
            ++selectedOption;
        } else if (event.key.code == sf::Keyboard::Enter ||
                   event.key.code == sf::Keyboard::Space) {
            activateSelection();
        }
    }

    int optionCount = 1;
    switch (screen) {
        case Screen::MainMenu: optionCount = 5; break;
        case Screen::LevelSelect: optionCount = 3; break;
        case Screen::HowToPlay: optionCount = 1; break;
        case Screen::Settings: optionCount = 2; break;
        case Screen::ConfirmExit: optionCount = 2; break;
        case Screen::Paused: optionCount = 4; break;
        case Screen::LevelComplete: optionCount = 2; break;
        case Screen::Playing: break;
    }
    selectedOption = (selectedOption % optionCount + optionCount) % optionCount;
}

void Game::handleMenuClick(const sf::Vector2f& position) {
    if (screen == Screen::Playing) {
        return;
    }

    int optionCount = 1;
    switch (screen) {
        case Screen::MainMenu: optionCount = 5; break;
        case Screen::LevelSelect: optionCount = 3; break;
        case Screen::HowToPlay: optionCount = 1; break;
        case Screen::Settings: optionCount = 2; break;
        case Screen::ConfirmExit: optionCount = 2; break;
        case Screen::Paused: optionCount = 4; break;
        case Screen::LevelComplete: optionCount = 2; break;
        case Screen::Playing: return;
    }

    const sf::Vector2u size = window.getSize();
    const float scale = std::min(static_cast<float>(size.x) / 900.f,
                                 static_cast<float>(size.y) / 540.f);
    for (int i = 0; i < optionCount; ++i) {
        sf::FloatRect bounds = buttonBounds(size, i, optionCount, scale);
        if (screen == Screen::HowToPlay) {
            bounds.top = static_cast<float>(size.y) / 2.f + 105.f * scale;
        }
        if (bounds.contains(position.x, position.y)) {
            selectedOption = i;
            activateSelection();
            return;
        }
    }
}

void Game::activateSelection() {
    switch (screen) {
        case Screen::MainMenu:
            if (selectedOption == 0) startLevel(1);
            else if (selectedOption == 1) {
                screen = Screen::LevelSelect;
                selectedOption = 0;
            } else if (selectedOption == 2) {
                screen = Screen::HowToPlay;
                selectedOption = 0;
            } else if (selectedOption == 3) {
                screen = Screen::Settings;
                selectedOption = 0;
            } else {
                screen = Screen::ConfirmExit;
                selectedOption = 1;
            }
            break;
        case Screen::LevelSelect:
            if (selectedOption < 2) startLevel(selectedOption + 1);
            else {
                screen = Screen::MainMenu;
                selectedOption = 1;
            }
            break;
        case Screen::HowToPlay:
            screen = Screen::MainMenu;
            selectedOption = 2;
            break;
        case Screen::Settings:
            if (selectedOption == 0) toggleFullscreen();
            else {
                screen = Screen::MainMenu;
                selectedOption = 3;
            }
            break;
        case Screen::ConfirmExit:
            if (selectedOption == 0) window.close();
            else {
                screen = Screen::MainMenu;
                selectedOption = 0;
            }
            break;
        case Screen::Playing:
            break;
        case Screen::Paused:
            if (selectedOption == 0) screen = Screen::Playing;
            else if (selectedOption == 1) startLevel(currentLevel);
            else if (selectedOption == 2) {
                screen = Screen::MainMenu;
                selectedOption = 0;
            } else {
                window.close();
            }
            break;
        case Screen::LevelComplete:
            if (selectedOption == 0 && currentLevel == 1) startLevel(2);
            else {
                screen = Screen::MainMenu;
                selectedOption = 0;
            }
            break;
    }
}

void Game::switchForm() {
    const sf::FloatRect bounds = player->getBounds();
    const sf::Vector2f currentPosition(bounds.left, bounds.top);
    std::unique_ptr<Character> nextForm;
    if (isHuman) nextForm = std::make_unique<Ghost>(currentPosition);
    else nextForm = std::make_unique<Human>(currentPosition);

    for (const auto& obstacle : obstacles) {
        if (obstacle->blocks(*nextForm) && bounds.intersects(obstacle->getBounds())) {
            return;
        }
    }
    for (const auto& door : doors) {
        if (door->blocks() && bounds.intersects(door->getBounds())) {
            return;
        }
    }

    const bool wasGrounded = player->isGrounded();
    const float previousVelocity = player->getVerticalVelocity();
    player = std::move(nextForm);
    player->setVerticalVelocity(previousVelocity);
    player->setGrounded(wasGrounded && !player->canFly());
    isHuman = !isHuman;
}

void Game::update(float deltaTime) {
    visualTime += deltaTime;
    if (screen != Screen::Playing) {
        return;
    }

    float direction = 0.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
        direction -= 1.f;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
        direction += 1.f;
    }
    player->setMovementDirection(direction);
    player->update(deltaTime);

    const float dx = player->getHorizontalVelocity() * deltaTime;
    if (player->canFly()) {
        float verticalDirection = 0.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
            verticalDirection -= 1.f;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
            verticalDirection += 1.f;
        }

        const sf::FloatRect bounds = player->getBounds();
        const sf::Vector2u windowSize = window.getSize();
        const float targetX = std::max(0.f, std::min(
            bounds.left + dx, static_cast<float>(windowSize.x) - bounds.width));
        player->move(targetX - bounds.left, 0.f);
        for (const auto& obstacle : obstacles) {
            if (obstacle->isTrampoline() || !obstacle->blocks(*player)) continue;
            const sf::FloatRect playerBounds = player->getBounds();
            const sf::FloatRect obstacleBounds = obstacle->getBounds();
            if (playerBounds.intersects(obstacleBounds)) {
                if (dx > 0.f) {
                    player->move(obstacleBounds.left - playerBounds.left - playerBounds.width, 0.f);
                } else if (dx < 0.f) {
                    player->move(obstacleBounds.left + obstacleBounds.width - playerBounds.left, 0.f);
                }
            }
        }
        for (const auto& door : doors) {
            if (!door->blocks()) continue;
            const sf::FloatRect playerBounds = player->getBounds();
            const sf::FloatRect doorBounds = door->getBounds();
            if (playerBounds.intersects(doorBounds)) {
                if (dx > 0.f) {
                    player->move(doorBounds.left - playerBounds.left - playerBounds.width, 0.f);
                } else if (dx < 0.f) {
                    player->move(doorBounds.left + doorBounds.width - playerBounds.left, 0.f);
                }
            }
        }

        const sf::FloatRect horizontalBounds = player->getBounds();
        const float clampedY = std::max(0.f, std::min(
            horizontalBounds.top + verticalDirection * player->getMovementSpeed() * deltaTime,
            static_cast<float>(windowSize.y) - horizontalBounds.height));
        player->move(0.f, clampedY - horizontalBounds.top);
        for (const auto& obstacle : obstacles) {
            if (obstacle->isTrampoline() || !obstacle->blocks(*player)) continue;
            const sf::FloatRect playerBounds = player->getBounds();
            const sf::FloatRect obstacleBounds = obstacle->getBounds();
            if (playerBounds.intersects(obstacleBounds)) {
                if (verticalDirection > 0.f) {
                    player->move(0.f, obstacleBounds.top - playerBounds.top - playerBounds.height);
                } else if (verticalDirection < 0.f) {
                    player->move(0.f, obstacleBounds.top + obstacleBounds.height - playerBounds.top);
                }
            }
        }
        for (const auto& door : doors) {
            if (!door->blocks()) continue;
            const sf::FloatRect playerBounds = player->getBounds();
            const sf::FloatRect doorBounds = door->getBounds();
            if (playerBounds.intersects(doorBounds)) {
                if (verticalDirection > 0.f) {
                    player->move(0.f, doorBounds.top - playerBounds.top - playerBounds.height);
                } else if (verticalDirection < 0.f) {
                    player->move(0.f, doorBounds.top + doorBounds.height - playerBounds.top);
                }
            }
        }
        player->setVerticalVelocity(0.f);
        player->setGrounded(false);
    } else {
        player->move(dx, 0.f);
        for (const auto& obstacle : obstacles) {
            const sf::FloatRect bounds = player->getBounds();
            const sf::FloatRect obstacleBounds = obstacle->getBounds();
            if (bounds.intersects(obstacleBounds) && obstacle->blocks(*player)) {
                if (dx > 0.f) {
                    player->move(obstacleBounds.left - bounds.left - bounds.width, 0.f);
                } else if (dx < 0.f) {
                    player->move(obstacleBounds.left + obstacleBounds.width - bounds.left, 0.f);
                }
            }
        }
        for (const auto& door : doors) {
            if (!door->blocks()) continue;
            const sf::FloatRect bounds = player->getBounds();
            const sf::FloatRect doorBounds = door->getBounds();
            if (bounds.intersects(doorBounds)) {
                if (dx > 0.f) {
                    player->move(doorBounds.left - bounds.left - bounds.width, 0.f);
                } else if (dx < 0.f) {
                    player->move(doorBounds.left + doorBounds.width - bounds.left, 0.f);
                }
            }
        }

        const bool wasGrounded = player->isGrounded();
        player->setGrounded(false);
        player->applyGravity(deltaTime);
        const float dy = player->getVerticalVelocity() * deltaTime;
        const sf::FloatRect previousBounds = player->getBounds();
        player->move(0.f, dy);

        for (const auto& obstacle : obstacles) {
            if (!obstacle->isTrampoline()) continue;
            const sf::FloatRect bounds = player->getBounds();
            const sf::FloatRect trampolineBounds = obstacle->getBounds();
            const float previousBottom = previousBounds.top + previousBounds.height;
            if (dy >= 0.f && bounds.intersects(trampolineBounds) &&
                (previousBottom <= trampolineBounds.top + 1.f || wasGrounded)) {
                player->move(0.f, trampolineBounds.top - bounds.top - bounds.height);
                player->bounceFromTrampoline();
                break;
            }
        }

        for (const auto& obstacle : obstacles) {
            if (obstacle->isTrampoline()) continue;
            const sf::FloatRect bounds = player->getBounds();
            const sf::FloatRect obstacleBounds = obstacle->getBounds();
            if (bounds.intersects(obstacleBounds) && obstacle->blocks(*player)) {
                if (dy > 0.f) {
                    player->move(0.f, obstacleBounds.top - bounds.top - bounds.height);
                    player->setGrounded(true);
                } else if (dy < 0.f) {
                    player->move(0.f, obstacleBounds.top + obstacleBounds.height - bounds.top);
                    player->stopVerticalMovement();
                }
            }
        }
        for (const auto& door : doors) {
            if (!door->blocks()) continue;
            const sf::FloatRect bounds = player->getBounds();
            const sf::FloatRect doorBounds = door->getBounds();
            if (bounds.intersects(doorBounds)) {
                if (dy > 0.f) {
                    player->move(0.f, doorBounds.top - bounds.top - bounds.height);
                    player->setGrounded(true);
                } else if (dy < 0.f) {
                    player->move(0.f, doorBounds.top + doorBounds.height - bounds.top);
                    player->stopVerticalMovement();
                }
            }
        }
    }

    for (auto& obstacle : obstacles) obstacle->update(deltaTime);
    const bool platePressed = player->canActivateSwitch() &&
        !pressurePlates.empty() &&
        player->getBounds().intersects(pressurePlates.front()->getBounds());
    if (!pressurePlates.empty()) {
        pressurePlates.front()->setPressed(platePressed);
        pressurePlates.front()->update(deltaTime);
    }

    if (platePressed) {
        doorCloseTimer = 3.f;
        doors.front()->setOpen(true);
    } else if (!doors.empty() && doors.front()->isOpen()) {
        doorCloseTimer = std::max(0.f, doorCloseTimer - deltaTime);
        if (doorCloseTimer == 0.f) doors.front()->setOpen(false);
    }
    for (auto& door : doors) door->update(deltaTime);

    if (!doors.empty() && doors.front()->isOpen() &&
        player->getBounds().intersects(doors.front()->getBounds())) {
        screen = Screen::LevelComplete;
        selectedOption = 0;
    }
}

void Game::renderHud() {
    const sf::Vector2u size = window.getSize();
    sf::RectangleShape panel(sf::Vector2f(static_cast<float>(size.x), 42.f));
    panel.setFillColor(sf::Color(10, 15, 29, 220));
    window.draw(panel);

    const float scale = std::min(static_cast<float>(size.x) / 900.f,
                                 static_cast<float>(size.y) / 540.f);
    drawPixelText(window, currentLevel == 1 ? "LEVEL 1 - THE VEIL" : "LEVEL 2 - THE DEPTHS",
        18.f * scale, 16.f * scale, 1.8f * scale, sf::Color(216, 229, 245));
    drawPixelText(window, isHuman ? "HUMAN FORM" : "GHOST FORM",
        static_cast<float>(size.x) - 190.f * scale, 16.f * scale,
        1.8f * scale, isHuman ? sf::Color(255, 190, 112) : sf::Color(110, 220, 255));

    sf::RectangleShape hintPanel(sf::Vector2f(static_cast<float>(size.x), 28.f * scale));
    hintPanel.setPosition(0.f, static_cast<float>(size.y) - 28.f * scale);
    hintPanel.setFillColor(sf::Color(10, 15, 29, 205));
    window.draw(hintPanel);
    drawPixelText(window, "A D MOVE   SPACE JUMP   G SHIFT   ESC PAUSE",
        static_cast<float>(size.x) / 2.f,
        static_cast<float>(size.y) - 19.f * scale, 1.45f * scale,
        sf::Color(181, 196, 221), true);
}

void Game::renderMenu() {
    window.setView(window.getDefaultView());
    drawBackdrop(window, visualTime);

    const sf::Vector2u size = window.getSize();
    const float scale = std::min(static_cast<float>(size.x) / 900.f,
                                 static_cast<float>(size.y) / 540.f);
    const float centerX = static_cast<float>(size.x) / 2.f;
    const float centerY = static_cast<float>(size.y) / 2.f;

    sf::CircleShape seal(38.f * scale, 6);
    seal.setOrigin(38.f * scale, 38.f * scale);
    seal.setPosition(centerX, static_cast<float>(size.y) * 0.19f);
    seal.setRotation(30.f);
    seal.setFillColor(sf::Color(20, 35, 57, 150));
    seal.setOutlineColor(sf::Color(109, 202, 228, 170));
    seal.setOutlineThickness(2.f * scale);
    window.draw(seal);

    sf::CircleShape core(10.f * scale, 6);
    core.setOrigin(10.f * scale, 10.f * scale);
    core.setPosition(centerX, static_cast<float>(size.y) * 0.19f);
    core.setRotation(-visualTime * 28.f);
    core.setFillColor(sf::Color(108, 215, 238, 220));
    window.draw(core);

    drawPixelText(window, "SOUL SHIFT", centerX, static_cast<float>(size.y) * 0.29f,
        5.f * scale, sf::Color(236, 244, 255), true);
    drawPixelText(window, "TWO SOULS. ONE DESTINY.", centerX,
        static_cast<float>(size.y) * 0.29f + 48.f * scale,
        1.8f * scale, sf::Color(146, 196, 220), true);

    std::array<std::string, 5> mainOptions{{
        "START GAME", "LEVEL SELECT", "HOW TO PLAY", "SETTINGS", "EXIT GAME"}};
    std::array<std::string, 3> levelOptions{{
        "LEVEL 1 - THE VEIL", "LEVEL 2 - THE DEPTHS", "BACK"}};
    std::array<std::string, 1> backOption{{"BACK"}};
    std::array<std::string, 2> settingsOptions{{
        fullscreen ? "FULLSCREEN: ON" : "FULLSCREEN: OFF", "BACK"}};
    std::array<std::string, 2> confirmOptions{{"YES - EXIT", "NO - RETURN"}};
    std::array<std::string, 4> pauseOptions{{
        "RESUME", "RESTART LEVEL", "MAIN MENU", "EXIT GAME"}};
    std::array<std::string, 2> completeOptions{{
        currentLevel == 1 ? "CONTINUE TO LEVEL 2" : "RETURN TO MENU", "MAIN MENU"}};

    int optionCount = 0;
    const std::string* options = nullptr;
    switch (screen) {
        case Screen::MainMenu:
            options = mainOptions.data();
            optionCount = static_cast<int>(mainOptions.size());
            break;
        case Screen::LevelSelect:
            options = levelOptions.data();
            optionCount = static_cast<int>(levelOptions.size());
            break;
        case Screen::HowToPlay:
            options = backOption.data();
            optionCount = 1;
            break;
        case Screen::Settings:
            options = settingsOptions.data();
            optionCount = static_cast<int>(settingsOptions.size());
            break;
        case Screen::ConfirmExit:
            options = confirmOptions.data();
            optionCount = static_cast<int>(confirmOptions.size());
            break;
        case Screen::Paused:
            options = pauseOptions.data();
            optionCount = static_cast<int>(pauseOptions.size());
            break;
        case Screen::LevelComplete:
            options = completeOptions.data();
            optionCount = static_cast<int>(completeOptions.size());
            break;
        case Screen::Playing:
            break;
    }

    if (screen == Screen::HowToPlay) {
        drawPixelText(window, "HUMAN: WALK JUMP PUSH AND USE PLATES",
            centerX, centerY - 94.f * scale, 1.75f * scale,
            sf::Color(255, 190, 112), true);
        drawPixelText(window, "GHOST: FLOAT AND CROSS MAGIC BARRIERS",
            centerX, centerY - 57.f * scale, 1.75f * scale,
            sf::Color(110, 220, 255), true);
        drawPixelText(window, "A D OR ARROWS: MOVE   SPACE: JUMP",
            centerX, centerY - 7.f * scale, 1.65f * scale,
            sf::Color(208, 220, 237), true);
        drawPixelText(window, "G: CHANGE FORM   ESC: PAUSE",
            centerX, centerY + 27.f * scale, 1.65f * scale,
            sf::Color(208, 220, 237), true);
        drawPixelText(window, "HUMAN ONLY CAN ACTIVATE THE PRESSURE PLATE",
            centerX, centerY + 61.f * scale, 1.45f * scale,
            sf::Color(162, 181, 209), true);
    } else if (screen == Screen::LevelSelect) {
        drawPixelText(window, "CHOOSE YOUR PATH",
            centerX, centerY - 64.f * scale, 2.f * scale,
            sf::Color(200, 219, 239), true);
    } else if (screen == Screen::Settings) {
        drawPixelText(window, "DISPLAY MODE", centerX, centerY - 91.f * scale,
            1.7f * scale, sf::Color(189, 207, 229), true);
        drawPixelText(window, "AUDIO IS NOT AVAILABLE IN THIS BUILD",
            centerX, centerY - 59.f * scale, 1.35f * scale,
            sf::Color(150, 169, 197), true);
    } else if (screen == Screen::LevelComplete) {
        drawPixelText(window, currentLevel == 1
            ? "THE FIRST SEAL IS BROKEN" : "BOTH REALMS ARE AT PEACE",
            centerX, centerY - 96.f * scale, 1.6f * scale,
            sf::Color(182, 232, 210), true);
    } else if (screen == Screen::Paused) {
        drawPixelText(window, currentLevel == 1 ? "LEVEL 1 - THE VEIL" : "LEVEL 2 - THE DEPTHS",
            centerX, centerY - 139.f * scale, 1.6f * scale,
            sf::Color(173, 194, 221), true);
    } else if (screen == Screen::ConfirmExit) {
        drawPixelText(window, "YOUR PROGRESS WILL BE LOST",
            centerX, centerY - 68.f * scale, 1.45f * scale,
            sf::Color(177, 191, 215), true);
    }

    const sf::Vector2i mouse = sf::Mouse::getPosition(window);
    for (int i = 0; i < optionCount; ++i) {
        sf::FloatRect bounds = buttonBounds(size, i, optionCount, scale);
        if (screen == Screen::HowToPlay) {
            bounds.top = centerY + 105.f * scale;
        }
        const bool hover = bounds.contains(static_cast<float>(mouse.x),
                                           static_cast<float>(mouse.y));
        drawButton(window, bounds, options[i], scale, i == selectedOption || hover);
    }

    if (screen == Screen::MainMenu) {
        drawPixelText(window, "ARROWS / W S SELECT     ENTER OR CLICK CONFIRM",
            centerX, static_cast<float>(size.y) - 25.f * scale,
            1.2f * scale, sf::Color(133, 153, 182), true);
    } else {
        drawPixelText(window, "ARROWS / W S SELECT     ENTER CONFIRM     ESC BACK",
            centerX, static_cast<float>(size.y) - 25.f * scale,
            1.1f * scale, sf::Color(133, 153, 182), true);
    }
}

void Game::render() {
    window.clear(sf::Color(16, 20, 39));
    if (screen != Screen::Playing) {
        renderMenu();
        window.display();
        return;
    }

    drawBackdrop(window, visualTime);
    for (const auto& obstacle : obstacles) obstacle->render(window);
    for (const auto& plate : pressurePlates) plate->render(window);
    for (const auto& door : doors) door->render(window);
    player->render(window);
    renderHud();
    window.display();
}

void Game::run() {
    sf::Clock clock;
    while (window.isOpen()) {
        const float deltaTime = std::min(clock.restart().asSeconds(), 0.05f);
        processEvents();
        update(deltaTime);
        render();
    }
}


// ===== Program entry point =====
int main() {
    Game game;
    game.run();
    return 0;
}
