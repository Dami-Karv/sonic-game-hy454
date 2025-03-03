#include "SpringSprite.h"
#include <iostream>

SpringSprite::SpringSprite(const sf::Vector2f& pos) : BaseSprite(pos, false) {
    if (!texture.loadFromFile("./assets/misc_fixed.png")) {
        std::cerr << "fail" << std::endl;
        return;
    }

    initializeFrames();
    sprite.setTextureRect(m_normalFrame);
    sprite.setPosition(position);
}

void SpringSprite::initializeFrames() {

    m_normalFrame = sf::IntRect(NORMAL_FRAME_POS, NORMAL_FRAME_SIZE);

    m_extendedFrame = sf::IntRect(EXTENDED_FRAME_POS, EXTENDED_FRAME_SIZE);
}

void SpringSprite::extend() {
    if (m_canBounce && !m_extended) {
        m_extended = true;
        m_canBounce = false;
        m_extensionTimer = 0.0f;


        float heightDiff = static_cast<float>(EXTENDED_FRAME_SIZE.y - NORMAL_FRAME_SIZE.y);
        sprite.setPosition(sf::Vector2f(position.x, position.y - heightDiff));
        sprite.setTextureRect(m_extendedFrame);
    }
}

void SpringSprite::update(float deltaTime) {
    if (m_extended) {
        m_extensionTimer += deltaTime;
        if (m_extensionTimer >= EXTENSION_DURATION) {

            m_extended = false;
            m_canBounce = true;
            sprite.setPosition(position);
            sprite.setTextureRect(m_normalFrame);
        }
    }
}

void SpringSprite::createSpringGroup(std::vector<SpringSprite*>& springs,
                                     const std::vector<sf::Vector2f>& positions) {
    for (const auto& pos : positions) {
        springs.push_back(new SpringSprite(pos));
    }
}