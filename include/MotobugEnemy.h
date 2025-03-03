#ifndef MOTOBUGENEMY_H
#define MOTOBUGENEMY_H

#include "BaseSprite.h"
#include "GameMap.h"
#include "AnimalSprite.h"

class MotobugEnemy final : public BaseSprite {
private:
    void initializeFrames() override;
    float movementTimer = 0.0f;
    float pauseTimer = 0.0f;
    float originalX;
    bool movingRight = true;
    bool isActive = true;
    GameMap* collisionMap = nullptr;
    std::vector<sf::IntRect> smokeFrames;
    int currentSmokeFrame = 0;
    float smokeAnimTimer = 0.0f;
    static constexpr float SMOKE_FRAME_TIME = 0.1f;
    std::unique_ptr<AnimalSprite> freedAnimal;


    static constexpr float TURN_PAUSE_DURATION = 0.5f; // Seconds to pause when turning

public:
    explicit MotobugEnemy(const sf::Vector2f& pos);
    ~MotobugEnemy() override = default;

    void update(float deltaTime) override;
    void updatePatrolMovement(float deltaTime);
    void die();
    bool isAlive() const;
    sf::FloatRect getCollisionBounds() const;
    void setCollisionMap(GameMap* map) { collisionMap = map; }

    AnimalSprite* getFreedAnimal() const { return freedAnimal.get(); }

    void render(sf::RenderWindow &window) const;
    void reset();
};

#endif