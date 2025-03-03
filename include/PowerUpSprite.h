#ifndef POWERUPSPRITE_H
#define POWERUPSPRITE_H

#include "BaseSprite.h"


class GameEngine;
class Player;

class PowerUpSprite final : public BaseSprite {
public:
    enum class PowerUpType {
        RINGS,
        INVINCIBILITY,
        SPEED,
        SHIELD,
        HEALTH
    };

    PowerUpSprite(const sf::Vector2f& pos, PowerUpType powerupType);
    ~PowerUpSprite() override = default;


    bool checkCollision(const sf::FloatRect& bounds, bool isBallState = false) const;
    void breakBox();


    sf::FloatRect getCollisionBounds() const;
    void render(sf::RenderWindow& window) override;

    void setEngineRef(GameEngine* engine) { engineRef = engine; }

private:
    void initializeFrames() override;
    PowerUpType type;
    bool broken{false};

    GameEngine* engineRef{nullptr};
};

#endif