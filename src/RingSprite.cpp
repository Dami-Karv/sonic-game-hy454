#include "../include/RingSprite.h"
#include "SoundManager.h"
#include <iostream>

RingSprite::RingSprite(const sf::Vector2f& pos)
        : BaseSprite(pos, true)
{
    if (!texture.loadFromFile("./assets/misc_fixed.png")) {
        std::cerr << "fail" << std::endl;
        return;
    }

    sprite.setTexture(texture);
    initializeFrames();
    initializeCollectFrames();
    sprite.setTextureRect(frames[0]);
    sprite.setPosition(position);
    frameTime = 0.1f;


    SoundManager::getInstance().loadSound("ring-collect", "./assets/ring-collect.mp3");
}

void RingSprite::initializeFrames() {
    frames.emplace_back(sf::IntRect(sf::Vector2i(8, 25), sf::Vector2i(16, 16)));
    frames.emplace_back(sf::IntRect(sf::Vector2i(32, 25), sf::Vector2i(16, 16)));
    frames.emplace_back(sf::IntRect(sf::Vector2i(56, 25), sf::Vector2i(8, 16)));
    frames.emplace_back(sf::IntRect(sf::Vector2i(72, 25), sf::Vector2i(16, 16)));
}

void RingSprite::initializeCollectFrames() {
    collectFrames.emplace_back(sf::IntRect(sf::Vector2i(96, 25), sf::Vector2i(16, 16)));
    collectFrames.emplace_back(sf::IntRect(sf::Vector2i(120, 25), sf::Vector2i(16, 16)));
    collectFrames.emplace_back(sf::IntRect(sf::Vector2i(144, 25), sf::Vector2i(16, 16)));
    collectFrames.emplace_back(sf::IntRect(sf::Vector2i(168, 25), sf::Vector2i(16, 16)));
}

void RingSprite::update(float deltaTime) {
    if (m_isCollected) {
        animationTimer += deltaTime;
        if (animationTimer >= frameTime) {
            animationTimer = 0.0f;
            collectFrameIndex++;

            if (collectFrameIndex >= static_cast<int>(collectFrames.size())) {  // Fixed sign comparison
                isCollectAnimationDone = true;
                return;
            }

            sprite.setTextureRect(collectFrames[collectFrameIndex]);
        }
    } else {
        BaseSprite::update(deltaTime);
    }
}

void RingSprite::collect() {
    if (!m_isCollected) {
        m_isCollected = true;
        collectFrameIndex = 0;
        animationTimer = 0.0f;
        sprite.setTextureRect(collectFrames[0]);


        SoundManager::getInstance().playSound("ring-collect");
    }
}

void RingSprite::createRingGroup(std::vector<RingSprite*>& sprites, float startX, float startY, int count) {
    for (int i = 0; i < count; ++i) {
        float xPos = startX + (i * 6 * 4);
        sprites.push_back(new RingSprite(sf::Vector2f(xPos, startY)));
    }
}

