#include "PlatformSprite.h"
#include <iostream>

PlatformSprite::PlatformSprite(const sf::Vector2f& pos)
        : BaseSprite(pos, false)  // Not animated
{
    if (!texture.loadFromFile("./assets/misc_fixed.png")) {
        std::cerr << "fail" << std::endl;
        return;
    }

    initializeFrames();
    sprite.setTextureRect(frames[0]);
    sprite.setPosition(position);
}

void PlatformSprite::initializeFrames() {
    frames.emplace_back(sf::IntRect(
            SPRITE_POS,
            SPRITE_SIZE
    ));
}

void PlatformSprite::createPlatformGroup(std::vector<PlatformSprite*>& platforms,
                                         const std::vector<sf::Vector2f>& positions) {
    for (const auto& pos : positions) {
        platforms.push_back(new PlatformSprite(pos));
    }
}