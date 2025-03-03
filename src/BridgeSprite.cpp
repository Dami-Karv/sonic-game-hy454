#include "../include/BridgeSprite.h"
#include <iostream>

BridgeSprite::BridgeSprite(const sf::Vector2f& pos)
        : BaseSprite(pos, false)
{
    if (!texture.loadFromFile("./assets/misc_fixed.png")) {
        std::cerr << "Fail" << std::endl;
        return;
    }

    sprite.setTexture(texture);
    initializeFrames();
    sprite.setTextureRect(frames[0]);
    sprite.setPosition(position);
    sprite.setScale(sf::Vector2f(1.0f, 1.0f));
}

void BridgeSprite::initializeFrames() {
    frames.emplace_back(sf::IntRect(
            sf::Vector2i(192, 25),
            sf::Vector2i(16, 16)
    ));
}

// Create a group of bridge sprites in a row
void BridgeSprite::createBridgeGroup(std::vector<BridgeSprite*>& sprites, float startX, float startY, int count) {
    for (int i = 0; i < count; ++i) {
        float xPos = startX + (i * 16);
        sprites.push_back(new BridgeSprite(sf::Vector2f(xPos, startY)));
    }
}