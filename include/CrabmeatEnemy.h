#ifndef CRABMEATENEMY_H
#define CRABMEATENEMY_H

#include "BaseSprite.h"
#include "GameMap.h"
#include "AnimalSprite.h"

class CrabmeatEnemy final : public BaseSprite {
private:
    void initializeFrames() override;
    float pauseTimer = 0.0f;
    float originalX;
    bool movingRight = true;
    bool isActive = true;
    GameMap* collisionMap = nullptr;
    std::unique_ptr<AnimalSprite> freedAnimal;


    float animationTimer = 0.0f;
    int currentFrame = 0;
    static constexpr float FRAME_TIME = 0.15f;


    std::vector<sf::IntRect> walkFrames;
    std::vector<sf::IntRect> attackFrames;

    static constexpr float TURN_PAUSE_DURATION = 0.5f;

    void updateAnimation(float deltaTime);

public:
    explicit CrabmeatEnemy(const sf::Vector2f& pos);
    ~CrabmeatEnemy() override = default;
    AnimalSprite* getFreedAnimal() const { return freedAnimal.get(); }
    void update(float deltaTime) override;
    void updatePatrolMovement(float deltaTime);
    void die();
    bool isAlive() const;
    sf::FloatRect getCollisionBounds() const;
    void setCollisionMap(GameMap* map) { collisionMap = map; }
    void render(sf::RenderWindow& window) const ;


    struct Projectile {
        sf::Vector2f position;
        sf::Vector2f velocity;
        bool active = true;
        static constexpr float INITIAL_SPEED = 200.0f;
        static constexpr float GRAVITY = 400.0f;
    };

    std::vector<Projectile> projectiles;
    sf::IntRect projectileFrame = sf::IntRect({120, 150}, {16, 16});  // Same as Buzzer's projectile
    bool isShooting = false;
    float shootingTimer = 0.0f;
    float attackCooldown = 0.0f;
    static constexpr float SHOOTING_DURATION = 0.7f;
    static constexpr float ATTACK_COOLDOWN = 3.0f;
    static constexpr float DETECTION_RANGE = 150.0f;
    bool checkProjectileCollision(const sf::FloatRect& bounds) const;
    bool checkPlayerInRange(const sf::FloatRect& playerBounds) const;
    void reset();

    void updateProjectiles(float deltaTime);

    void shoot();
};

#endif