#include "../include/MotobugEnemy.h"
#include "SoundManager.h"
#include <iostream>
#include <cmath>

MotobugEnemy::MotobugEnemy(const sf::Vector2f& pos)
        : BaseSprite(pos, true)
        , originalX(pos.x)
        , isActive(true)
        , movingRight(false)
{
    if (!texture.loadFromFile("./assets/enemies_sheet_fixed.png")) {
        std::cerr << "fail" << std::endl;
        return;
    }

    sprite.setTexture(texture);
    initializeFrames();
    sprite.setTextureRect(frames[0]);
    sprite.setPosition(position);
    frameTime = 0.1f;

    sprite.setScale(sf::Vector2f(1.f, 1.f));
    sprite.setOrigin(sf::Vector2f(0.f, 0.f));

    SoundManager::getInstance().loadSound("badnik-death", "./assets/badnik-death.mp3");
}

void MotobugEnemy::initializeFrames() {

    frames = {
            sf::IntRect(sf::Vector2i(157, 102), sf::Vector2i(40, 32)),
            sf::IntRect(sf::Vector2i(213, 102), sf::Vector2i(40, 32)),
            sf::IntRect(sf::Vector2i(157, 142), sf::Vector2i(40, 32)),
            sf::IntRect(sf::Vector2i(213, 142), sf::Vector2i(40, 32))
    };

    smokeFrames = {
            sf::IntRect(sf::Vector2i(269, 102), sf::Vector2i(8, 8)),
            sf::IntRect(sf::Vector2i(269, 118), sf::Vector2i(8, 8)),
            sf::IntRect(sf::Vector2i(269, 134), sf::Vector2i(8, 8))
    };
}

void MotobugEnemy::update(float deltaTime) {
    if (!isActive) {
        if (freedAnimal) {
            freedAnimal->update(deltaTime);
            if (freedAnimal->shouldBeDeleted()) {
                freedAnimal.reset();
            }
        }
        return;
    }

    smokeAnimTimer += deltaTime;
    if (smokeAnimTimer >= SMOKE_FRAME_TIME) {
        smokeAnimTimer = 0;
        currentSmokeFrame = (currentSmokeFrame + 1) % smokeFrames.size();
    }

    animationTimer += deltaTime;
    if (animationTimer >= frameTime) {
        animationTimer = 0;
        currentFrame = (currentFrame + 1) % frames.size();
        sprite.setTextureRect(frames[currentFrame]);
    }


    if (pauseTimer > 0) {
        pauseTimer -= deltaTime;
        return;
    }

    updatePatrolMovement(deltaTime);
}

void MotobugEnemy::updatePatrolMovement(float deltaTime) {
    const float MOVE_SPEED = 60.0f;
    float movement = MOVE_SPEED * deltaTime;


    sf::Vector2f prevPos = position;


    sf::Vector2f futurePos = position;
    if (movingRight) {
        futurePos.x += movement;
    } else {
        futurePos.x -= movement;
    }

    sf::FloatRect futureBounds = getCollisionBounds();
    futureBounds.position = futurePos;


    float groundCheckOffset = movingRight ? futureBounds.size.x + 5.0f : -5.0f;
    sf::FloatRect groundCheckRect(
            sf::Vector2f(futureBounds.position.x + groundCheckOffset, futureBounds.position.y + futureBounds.size.y),
            sf::Vector2f(2.0f, 32.0f)
    );

    bool shouldTurn = false;


    if (collisionMap && collisionMap->checkCollision(futureBounds, 0)) {
        shouldTurn = true;
    }

    if (!shouldTurn && collisionMap && !collisionMap->checkCollision(groundCheckRect, 0)) {

        groundCheckRect.position.x += (movingRight ? -10.0f : 10.0f);
        if (!collisionMap->checkCollision(groundCheckRect, 0)) {
            shouldTurn = true;
        }
    }

    if (shouldTurn && pauseTimer <= 0) {
        movingRight = !movingRight;
        position = prevPos;
        pauseTimer = TURN_PAUSE_DURATION;

        if (movingRight) {
            sprite.setScale(sf::Vector2f(-1.f, 1.f));
            sprite.setOrigin(sf::Vector2f(sprite.getGlobalBounds().size.x, 0.f));
        } else {
            sprite.setScale(sf::Vector2f(1.f, 1.f));
            sprite.setOrigin(sf::Vector2f(0.f, 0.f));
        }
    } else if (!shouldTurn) {
        position = futurePos;
    }

    sprite.setPosition(position);
}

void MotobugEnemy::render(sf::RenderWindow& window) const {
    if (!isActive) {
        if (freedAnimal) {
            freedAnimal->render(window);
        }
        return;
    }
    window.draw(sprite);


    sf::Sprite smokeSprite(texture);
    smokeSprite.setTextureRect(smokeFrames[currentSmokeFrame]);

    sf::Vector2f smokePos = position;
    if (movingRight) {
        smokePos.x -= 8.0f;
    } else {
        smokePos.x += sprite.getGlobalBounds().size.x;
    }
    smokePos.y += 11.0f;

    smokeSprite.setPosition(smokePos);
    window.draw(smokeSprite);
}

void MotobugEnemy::die() {
    if (isActive) {
        isActive = false;

        SoundManager::getInstance().playSound("badnik-death");


        bool moveRight = (std::rand() % 2) == 0;
        freedAnimal = std::make_unique<AnimalSprite>(position, moveRight);
        if (freedAnimal && collisionMap) {
            freedAnimal->setCollisionMap(collisionMap);
        }
    }
}

bool MotobugEnemy::isAlive() const {
    return isActive;
}

sf::FloatRect MotobugEnemy::getCollisionBounds() const {
    sf::FloatRect bounds = sprite.getGlobalBounds();


    float newWidth = bounds.size.x * 0.7f;
    float newHeight = bounds.size.y * 0.8f;


    float xOffset = (bounds.size.x - newWidth) * 0.5f;
    float yOffset = (bounds.size.y - newHeight) * 0.5f;

    return sf::FloatRect(
            bounds.position + sf::Vector2f(xOffset, yOffset),
            sf::Vector2f(newWidth, newHeight)
    );
}

void MotobugEnemy::reset() {

    position.x = originalX;
    isActive = true;
    movingRight = false;
    sprite.setScale(sf::Vector2f(1.f, 1.f));
    sprite.setOrigin(sf::Vector2f(0.f, 0.f));
    sprite.setPosition(position);
    sprite.setTextureRect(frames[0]);
    movementTimer = 0.0f;
    pauseTimer = 0.0f;
    animationTimer = 0.0f;
    smokeAnimTimer = 0.0f;
    currentFrame = 0;
    currentSmokeFrame = 0;
    freedAnimal.reset();
}