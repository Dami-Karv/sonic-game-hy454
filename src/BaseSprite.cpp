#include "../include/BaseSprite.h"
#include <iostream>

BaseSprite::BaseSprite(const sf::Vector2f& pos, bool animated)
        : position(pos)
        , isAnimated(animated)
        , sprite(texture)
{
    sprite.setPosition(position);

    sprite.setColor(sf::Color(255, 255, 255, 255));
}

void BaseSprite::setOpacity(uint8_t alpha) {
    sf::Color color = sprite.getColor();
    color.a = alpha;
    sprite.setColor(color);
}

void BaseSprite::update(float deltaTime) {
    if (!isAnimated || frames.empty()) return;

    animationTimer += deltaTime;
    if (animationTimer >= frameTime) {
        animationTimer = 0.0f;
        currentFrame = (currentFrame + 1) % frames.size();
        sprite.setTextureRect(frames[currentFrame]);
    }
}

//function to render the sprite
void BaseSprite::render(sf::RenderWindow& window) {
    window.draw(sprite);
}

//function to set the position of the sprite
void BaseSprite::setPosition(const sf::Vector2f& pos) {
    position = pos;
    sprite.setPosition(position);
}