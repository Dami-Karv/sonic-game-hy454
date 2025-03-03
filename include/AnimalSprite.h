#ifndef ANIMALSPRITE_H
#define ANIMALSPRITE_H

#include "BaseSprite.h"
#include "GameMap.h"

class AnimalSprite final : public BaseSprite {
private:
    float velocityY = -200.0f;
    float velocityX = 50.0f;
    static constexpr float GRAVITY = 200.0f;
    bool shouldDelete = false;
    GameMap* collisionMap = nullptr;
    void initializeFrames() override;
    float lifetimeSeconds = 0.0f;
    static constexpr float MAX_LIFETIME = 5.0f;

public:
    explicit AnimalSprite(const sf::Vector2f& pos, bool moveRight = true);
    ~AnimalSprite() override = default;

    void update(float deltaTime) override;
    bool shouldBeDeleted() const { return shouldDelete; }
    void setCollisionMap(GameMap* map) { collisionMap = map; }
};

#endif