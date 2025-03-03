#include "../include/SpikeSprite.h"
#include <iostream>

SpikeSprite::SpikeSprite(const sf::Vector2f& pos)
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
}

void SpikeSprite::initializeFrames() {
    frames.emplace_back(sf::IntRect(
            sf::Vector2i(308, 25),
            sf::Vector2i(40, 32)
    ));
}

sf::FloatRect SpikeSprite::getCollisionBounds() const {
    auto bounds = sprite.getGlobalBounds();
    float reduction = 4.0f;


    sf::Vector2f position(
            bounds.position.x + reduction,
            bounds.position.y + reduction
    );
    sf::Vector2f size(
            bounds.size.x - (2 * reduction),
            bounds.size.y - (2 * reduction)
    );

    return sf::FloatRect(position, size);
}




void SpikeSprite::createSpikeGroup(std::vector<SpikeSprite*>& sprites, const std::vector<sf::Vector2f>& positions) {
    for (const auto& pos : positions) {
        sprites.push_back(new SpikeSprite(pos));
    }
}