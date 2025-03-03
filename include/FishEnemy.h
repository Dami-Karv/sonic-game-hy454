#ifndef FISHENEMY_H
#define FISHENEMY_H

#include "BaseSprite.h"
#include "AnimalSprite.h"
#include "GameMap.h"

class Player;

class FishEnemy final : public BaseSprite {
private:
    void initializeFrames() override;
    float movementTimer = 0.0f;
    float startY;
    float endY;
    std::unique_ptr<AnimalSprite> freedAnimal;
    bool movingUp = true;
    const float MOVEMENT_SPEED = 100.0f;
    bool isActive = true;
    GameMap* collisionMap = nullptr;

public:
    FishEnemy(const sf::Vector2f& pos, float targetY);
    ~FishEnemy() override = default;

    void update(float deltaTime) override;
    void updateVerticalMovement(float deltaTime);
    sf::FloatRect getCollisionBounds() const;
    void render(sf::RenderWindow& window) const;
    void setCollisionMap(GameMap* map) { collisionMap = map; }

    bool isAlive() const { return isActive; }
    void die();
    void handleCollision(Player* player);
    AnimalSprite* getFreedAnimal() const { return freedAnimal.get(); }
    void reset();
};

#endif // FISHENEMY_H