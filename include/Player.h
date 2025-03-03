#ifndef PLAYER_H
#define PLAYER_H

#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
#include "GameMap.h"
#include "GameEngine.h"
#include "GameState.h"
#include "SpikeSprite.h"
#include "../include/FishEnemy.h"

// Forward declarations
class GameEngine;

// Animation states enum
enum PLAYER_ANIMATION_STATES {
    IDLE,
    BORED,
    MOVING_LEFT,
    MOVING_RIGHT,
    LOOKING_UP,
    CURLING_DOWN,
    FULL_SPEED,
    JUMPING,
    FALLING,
    SKIDDING,
    PUSHING,
    PIPE_SLIDING
};

// Helper function for smooth movement
template<typename T>
T move_toward(T current, T target, T maxDelta) {
    if (std::abs(target - current) <= maxDelta)
        return target;
    return current + std::copysign(maxDelta, target - current);
}

class Player final {
public:
    // Constructor/Destructor
    Player();
    ~Player();

    void startSpeedBoost(float duration) {
        speedBoostTimer = duration;
        speedBoostMultiplier = 1.5f;
        currentMaxSpeed = TOP_SPEED * speedBoostMultiplier;
    }


    float invincibilityTimer = 0.0f;
    float speedBoostTimer = 0.0f;
    bool hasShield = false;
    float speedBoostMultiplier = 1.0f;

    void startInvincibility(float duration) {
        invincibilityTimer = duration;
        isInvincible = true;
    }

    void bounceFromSpring();



    void addShield() {
        hasShield = true;
        isInvincible = false;
    }

    void resetAfterDeath();

    bool checkTeleportArea() {

        if (isHurt || isDead) {
            return false;
        }


        struct TeleportZone {
            sf::FloatRect triggerBox;
            sf::Vector2f destination;
        };

        std::vector<TeleportZone> teleportZones = {

                {
                        sf::FloatRect(sf::Vector2f(6613.0f, 346.0f), sf::Vector2f(40.0f, 60.0f)),
                        sf::Vector2f(6993.0f, 1118.0f)
                },
                {
                        sf::FloatRect(sf::Vector2f(6612.0f, 852.0f), sf::Vector2f(40.0f, 60.0f)),
                        sf::Vector2f(6993.0f, 1118.0f)
                },
                {
                        sf::FloatRect(sf::Vector2f(6102.0f, 579.0f), sf::Vector2f(40.0f, 60.0f)),
                        sf::Vector2f(6413.79f, 850.5f)

                }
        };

        sf::FloatRect playerBounds = getCollisionBounds();

        for (const auto& zone : teleportZones) {
            if (zone.triggerBox.findIntersection(playerBounds)) {

                setPosition(zone.destination);


                velocity = sf::Vector2f(0.f, 0.f);
                groundSpeed = 0.0f;
                isJumping = false;
                isOnGround = false;
                isSkidding = false;
                controlLock = false;
                animState = IDLE;
                frame = sf::IntRect({27, 39}, {32, 40});
                sprite.setTextureRect(frame);

                return true;
            }
        }

        return false;
    }


    void update();
    void render(sf::RenderWindow& window) const;
    void resetGame();

    bool wasHurt = false;
    bool wasInvincible = false;

    bool isInBallState() const {
        return animState == JUMPING || animState == FALLING;
    }


    bool IsDead() const { return isDead; }
    bool IsHurt() const { return isHurt; }
    bool getAnimationSwitch();
    sf::Vector2f getPosition() const;
    sf::FloatRect getCollisionBounds() const;
    bool isHighWall() const;


    void setGodMode(bool enabled);
    void setCollisionMap(GameMap* map) { collisionMap = map; }
    void setPosition(float x, float y);
    void setPosition(const sf::Vector2f& pos) { setPosition(pos.x, pos.y); }
    void setEngineRef(GameEngine* engine) { engineRef = engine; }


    int getRingCount() const { return ringCount; }
    void addRing() { ringCount++; }
    void loseRings();
    void scatterRings(int ringsToScatter);


    void move(float dx, float dy);
    void resetAnimationTimer();
    void triggerGameCompletion();
    void checkSpikeCollisions(const std::vector<SpikeSprite*>& spikes);
    void checkDeathConditions();
    void triggerDeath();

    void handleVerticalMovement(sf::Vector2f& pos, sf::FloatRect& bounds, float stepSize);

    void handleDamage();
    bool IsInvincible() const { return isInvincible; }
    bool isInvincible = false;

    sf::Vector2f velocity;

    void setRingCount(int count) { ringCount = count; }

private:

    sf::SoundBuffer ringLossBuffer;
    sf::SoundBuffer deathBuffer;
    std::unique_ptr<sf::Sound> ringLossSound;
    std::unique_ptr<sf::Sound> deathSound;
    bool soundsLoaded = false;

    void initSounds() {

        if (!soundsLoaded) {
            if (ringLossBuffer.loadFromFile("./assets/ring-loss.mp3")) {
                ringLossSound = std::make_unique<sf::Sound>(ringLossBuffer);
            } else {
                std::cerr << "Failed to load ring loss sound" << std::endl;
            }

            if (deathBuffer.loadFromFile("./assets/death.mp3")) {
                deathSound = std::make_unique<sf::Sound>(deathBuffer);
            } else {
                std::cerr << "Failed to load death sound" << std::endl;
            }

            soundsLoaded = true;
        }
    }


    float currentMaxSpeed = TOP_SPEED;

    static constexpr float ACCELERATION_SPEED = 0.046875f;
    static constexpr float DECELERATION_SPEED = 0.5f;
    static constexpr float FRICTION_SPEED = 0.046875f;
    static constexpr float TOP_SPEED = 6.0f;
    static constexpr float AIR_ACCELERATION = 0.09375f;
    static constexpr float MIN_JUMP_SPEED = -4.0f;
    static constexpr float JUMP_FORCE = -12.0f;
    static constexpr float MAX_AIR_TOLERANCE = 0.15f;

    static constexpr float HURT_BOUNCE_VELOCITY_Y = -7.0f;
    static constexpr float HURT_GRAVITY = 0.5f;
    static constexpr float HURT_DURATION = 1.0f;
    static constexpr float INVINCIBILITY_DURATION = 2.0f;



    bool isDead = false;
    bool isHurt = false;
    bool isGodMode = false;
    bool isJumping = false;
    bool isOnGround = false;
    bool jumpButtonHeld = false;
    bool controlLock = false;
    bool isSkidding = false;
    bool animSwitch = false;

    sf::Vector2f previousPosition;
    sf::Vector2f normalSize;
    float groundSpeed = 0.0f;
    float controlLockTimer = 0.0f;
    float airTime = 0.0f;
    float skidThreshold = 4.0f;
    float skidDeceleration = 0.8f;
    float hurtTimer = 0.0f;


    float acc = 0.0f;
    float drag = 0.0f;
    float velocityMax = 0.0f;
    float velocityMin = 0.0f;
    float gravity = 0.0f;
    float gravityMax = 0.0f;
    float groundDrag = 0.0f;

    // Game state
    int ringCount = 0;
    short animState = IDLE;

    // SFML objects
    sf::Texture texture;
    sf::Sprite sprite;
    sf::IntRect frame;
    sf::Clock timer;
    sf::Clock idleClock;
    sf::Clock debugTimer;
    sf::Clock hurtClock;
    sf::Clock airTimer;


    GameMap* collisionMap = nullptr;
    GameEngine* engineRef = nullptr;


    void initPlayer();
    void initPhysics();
    void initAnimation();

    void updateMovement();
    void updateAnimation();
    void updatePhysics();
    void updateHurtState();
    void enterHurtState();
    void handleGodModeMovement();
    void handleGroundMovement(bool& isPushingWall);
    void handleRightMovement(bool& isPushingWall);
    void handleRegularRightMovement();
    void handleWallPush(bool& isPushingWall);
    void handleLeftMovement();
    void handleNoMovement();
    void handleAirMovement();
    void handleJumping();
    void startJump();
    void updateMovementAnimation(bool isPushingWall);
    void updateSpriteDirection(bool facingRight);


    sf::Vector2f getSpriteSize() const;


};

#endif