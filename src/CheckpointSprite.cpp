#include "../include/CheckpointSprite.h"
#include <iostream>

CheckpointSprite::CheckpointSprite(const sf::Vector2f& pos)
        : BaseSprite(pos, true)
{
    if (!texture.loadFromFile("./assets/misc_fixed.png")) {
        std::cerr << "Fail" << std::endl;
        return;
    }

    sprite.setTexture(texture);
    initializeFrames();
    sprite.setTextureRect(frames[0]);
    sprite.setPosition(position);
    frameTime = 0.1f;
}

//initialize the frames for the checkpoint
void CheckpointSprite::initializeFrames() {
    frames.emplace_back(sf::IntRect(sf::Vector2i(142, 313), sf::Vector2i(32, 64)));
    frames.emplace_back(sf::IntRect(sf::Vector2i(182, 313), sf::Vector2i(32, 64)));
    frames.emplace_back(sf::IntRect(sf::Vector2i(222, 313), sf::Vector2i(40, 64)));
    frames.emplace_back(sf::IntRect(sf::Vector2i(270, 313), sf::Vector2i(40, 64)));
    frames.emplace_back(sf::IntRect(sf::Vector2i(318, 313), sf::Vector2i(40, 64)));
    frames.emplace_back(sf::IntRect(sf::Vector2i(366, 313), sf::Vector2i(40, 64)));
    frames.emplace_back(sf::IntRect(sf::Vector2i(414, 313), sf::Vector2i(40, 64)));
    frames.emplace_back(sf::IntRect(sf::Vector2i(462, 313), sf::Vector2i(32, 64)));

    frames.emplace_back(sf::IntRect(sf::Vector2i(142, 387), sf::Vector2i(32, 64)));
    frames.emplace_back(sf::IntRect(sf::Vector2i(182, 387), sf::Vector2i(32, 64)));
    frames.emplace_back(sf::IntRect(sf::Vector2i(222, 387), sf::Vector2i(40, 64)));
    frames.emplace_back(sf::IntRect(sf::Vector2i(270, 387), sf::Vector2i(40, 64)));
    frames.emplace_back(sf::IntRect(sf::Vector2i(318, 387), sf::Vector2i(40, 64)));
    frames.emplace_back(sf::IntRect(sf::Vector2i(366, 387), sf::Vector2i(40, 64)));
    frames.emplace_back(sf::IntRect(sf::Vector2i(414, 387), sf::Vector2i(40, 64)));
    frames.emplace_back(sf::IntRect(sf::Vector2i(462, 387), sf::Vector2i(32, 64)));
}

void CheckpointSprite::update(float deltaTime) {
    if (!isActivated || hasPlayedAnimation) return;

    animationTimer += deltaTime;
    if (animationTimer >= frameTime) {
        animationTimer = 0.0f;
        currentFrame++;

        if (currentFrame >= frames.size()) {
            currentFrame = frames.size() - 1;
            hasPlayedAnimation = true;
        }

        sprite.setTextureRect(frames[currentFrame]);
    }
}

//function to activate the checkpoint
void CheckpointSprite::activate() {
    if (!isActivated) {
        isActivated = true;
        currentFrame = 0;
        animationTimer = 0.0f;
        hasPlayedAnimation = false;
    }
}

sf::FloatRect CheckpointSprite::getCollisionBounds() const {
    return sprite.getGlobalBounds();
}


void CheckpointSprite::createCheckpointGroup(std::vector<CheckpointSprite*>& sprites, const std::vector<sf::Vector2f>& positions) {
    for (const auto& pos : positions) {
        sprites.push_back(new CheckpointSprite(pos));
    }
}