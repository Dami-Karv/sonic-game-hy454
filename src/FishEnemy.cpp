#include "../include/FishEnemy.h"
#include "../include/Player.h"
#include "SoundManager.h"
#include <iostream>
#include <cmath>

FishEnemy::FishEnemy(const sf::Vector2f& pos, float targetY)
        : BaseSprite(pos, true)
        , startY(pos.y)
        , endY(targetY * 4)
{
    if (!texture.loadFromFile("./assets/enemies_sheet_fixed.png")) {
        std::cerr << "fail" << std::endl;
        return;
    }

    sprite.setTexture(texture);
    initializeFrames();
    sprite.setTextureRect(frames[0]);
    sprite.setPosition(position);
    frameTime = 0.2f;


    SoundManager::getInstance().loadSound("badnik-death", "./assets/badnik-death.mp3");
}

void FishEnemy::initializeFrames() {
    frames.emplace_back(sf::IntRect(sf::Vector2i(157, 209), sf::Vector2i(32, 32)));
    frames.emplace_back(sf::IntRect(sf::Vector2i(197, 209), sf::Vector2i(32, 32)));
}

void FishEnemy::update(float deltaTime) {
    if (!isActive) {
        if (freedAnimal) {
            freedAnimal->update(deltaTime);
            if (freedAnimal->shouldBeDeleted()) {
                freedAnimal.reset();
            }
        }
        return;
    }


    animationTimer += deltaTime;
    if (animationTimer >= frameTime) {
        currentFrame = (currentFrame + 1) % frames.size();
        sprite.setTextureRect(frames[currentFrame]);
        animationTimer = 0;
    }

    updateVerticalMovement(deltaTime);
}

void FishEnemy::updateVerticalMovement(float deltaTime) {
    float distance = std::abs(endY - startY);
    movementTimer += deltaTime;


    float normalizedPosition = (std::sin(movementTimer) + 1.0f) / 2.0f;
    position.y = startY + (normalizedPosition * distance);

    sprite.setPosition(position);
}

void FishEnemy::die() {
    if (isActive) {
        isActive = false;

        SoundManager::getInstance().playSound("badnik-death");

        bool moveRight = (std::rand() % 2) == 0;
        freedAnimal = std::make_unique<AnimalSprite>(position, moveRight);
        if (collisionMap) {
            freedAnimal->setCollisionMap(collisionMap);
        }
    }
}



sf::FloatRect FishEnemy::getCollisionBounds() const {
    return sprite.getGlobalBounds();
}

void FishEnemy::render(sf::RenderWindow& window) const {
    if (isActive) {
        window.draw(sprite);
    } else if (freedAnimal) {
        freedAnimal->render(window);
    }
}

void FishEnemy::reset() {

    position.y = startY;
    isActive = true;
    movingUp = true;

    sprite.setPosition(position);
    sprite.setTextureRect(frames[0]);

    currentFrame = 0;
    animationTimer = 0.0f;
    movementTimer = 0.0f;


    freedAnimal.reset();
}