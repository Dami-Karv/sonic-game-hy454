#include "AnimalSprite.h"
#include <iostream>
#include <cmath>


AnimalSprite::AnimalSprite(const sf::Vector2f& pos, bool moveRight) : BaseSprite(pos, true) {
    if (!texture.loadFromFile("./assets/animals_fixed.png")) {
        std::cerr << "fail" << std::endl;
        return;
    }

    sprite.setTexture(texture);
    initializeFrames();
    sprite.setTextureRect(frames[0]);
    sprite.setPosition(position);
    velocityX = moveRight ? 50.0f : -50.0f;
}


void AnimalSprite::initializeFrames() {
    frames.emplace_back(sf::IntRect(sf::Vector2i(17, 81), sf::Vector2i(16, 24)));
}

//updates the animal sprite
void AnimalSprite::update(float deltaTime) {

    lifetimeSeconds += deltaTime;
    if (lifetimeSeconds >= MAX_LIFETIME) {
        shouldDelete = true;
        return;
    }

    sf::Vector2f prevPos = position;

    velocityY += GRAVITY * deltaTime;
    position.y += velocityY * deltaTime;

    position.x += velocityX * deltaTime;

    if (collisionMap) {
        sf::FloatRect bounds = sprite.getGlobalBounds();
        bounds.position = position;

        if (collisionMap->checkCollision(bounds, velocityY)) {
            position.y = prevPos.y;
            velocityY = 0;


            velocityX *= 0.95f;
        }
    }

    sprite.setPosition(position);


    if (lifetimeSeconds > MAX_LIFETIME - 1.0f) {
        float alpha = std::abs(std::sin(lifetimeSeconds * 10.0f)) * 255.0f;
        sf::Color color = sprite.getColor();
        color.a = static_cast<uint8_t>(std::max(0.0f, std::min(255.0f, alpha)));
        sprite.setColor(color);
    }
}