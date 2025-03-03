#ifndef BUZZERENEMY_H
#define BUZZERENEMY_H

#include "BaseSprite.h"
#include "AnimalSprite.h"
#include "GameMap.h"

class BuzzerEnemy final : public BaseSprite {
private:
    void initializeFrames() override;
    float moveDistance;
    float originalX;
    bool movingRight;
    bool isActive = true;
    std::unique_ptr<AnimalSprite> freedAnimal;
    GameMap* collisionMap = nullptr;
    static constexpr float MOVE_SPEED = 0.667f;
    static constexpr float MAX_DISTANCE = 150.0f;

    bool isShooting = false;
    float shootingTimer = 0.0f;
    static constexpr float SHOOTING_DURATION = 0.7f;
    static constexpr float DETECTION_WIDTH = 180.0f;
    static constexpr float DETECTION_HEIGHT = 90.0f;

    struct Projectile {
        sf::Vector2f position;
        sf::Vector2f velocity;
        bool active = true;
        static constexpr float SPEED = 100.0f;
    };
    std::vector<Projectile> projectiles;

    sf::IntRect normalFrames[4];
    sf::IntRect shootingFrame;
    sf::IntRect projectileFrame;

    void updateProjectiles(float deltaTime);
    sf::FloatRect getDetectionBox() const;
    void updateAnimation(float deltaTime);
    void updateMovement(float deltaTime);

public:
    explicit BuzzerEnemy(const sf::Vector2f& pos);
    ~BuzzerEnemy() override = default;
    float attackCooldown = 0.0f;
    static constexpr float ATTACK_COOLDOWN = 3.0f;
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) const ;
    sf::FloatRect getCollisionBounds() const;
    void die();
    bool isAlive() const { return isActive; }
    void shoot(const sf::Vector2f& targetPos);
    bool checkPlayerInRange(const sf::FloatRect& playerBounds) const;
    bool checkProjectileCollision(const sf::FloatRect& bounds) const;
    void setCollisionMap(GameMap* map) { collisionMap = map; }
    AnimalSprite* getFreedAnimal() const { return freedAnimal.get(); }
    void reset();
};

#endif