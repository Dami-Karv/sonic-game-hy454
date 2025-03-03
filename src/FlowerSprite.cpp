#include "../include/FlowerSprite.h"
#include <iostream>

FlowerSprite::FlowerSprite(bool isMultiFrame, const sf::Vector2f& pos)
        : position(pos)
        , isMultiFrameFlower(isMultiFrame)
        , animationTimer(0.0f)
        , currentFrame(0)
        , sprite(texture)
{
    if (!texture.loadFromFile("./assets/flowers.png")) {
        std::cerr << "Fail" << std::endl;
        return;
    }

    sprite.setTexture(texture, true);
    sprite.setScale(sf::Vector2f(1.0f, 1.0f));
    initializeFrames(isMultiFrame);
    sprite.setPosition(position);
}

void FlowerSprite::initializeFrames(bool isMultiFrame) {
    if (isMultiFrame) {
        frames.emplace_back(sf::Vector2i(48, 48), sf::Vector2i(32, 40));
        frames.emplace_back(sf::Vector2i(88, 48), sf::Vector2i(32, 40));
        frames.emplace_back(sf::Vector2i(128, 48), sf::Vector2i(32, 40));
    } else {
        frames.emplace_back(sf::Vector2i(48, 8), sf::Vector2i(32, 32));
        frames.emplace_back(sf::Vector2i(88, 8), sf::Vector2i(32, 32));
    }

    if (!frames.empty()) {
        sprite.setTextureRect(frames[0]);
    }
}

void FlowerSprite::update(float deltaTime) {
    animationTimer += deltaTime;

    if (animationTimer >= frameTime) {
        animationTimer = 0.0f;
        currentFrame = (currentFrame + 1) % frames.size();
        sprite.setTextureRect(frames[currentFrame]);
    }
}

void FlowerSprite::render(sf::RenderWindow& window) {
    window.draw(sprite);

    static sf::Clock debugTimer;
    if (debugTimer.getElapsedTime().asSeconds() > 1.0f) {


        debugTimer.restart();
    }
}

void FlowerSprite::setPosition(const sf::Vector2f& pos) {
    position = pos;
    sprite.setPosition(position);
}