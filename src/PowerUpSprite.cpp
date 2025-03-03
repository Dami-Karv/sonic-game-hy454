#include "PowerUpSprite.h"
#include <iostream>

PowerUpSprite::PowerUpSprite(const sf::Vector2f& pos, PowerUpType powerupType)
        : BaseSprite(pos, true)
        , type(powerupType)
{
    if (!texture.loadFromFile("./assets/misc_fixed.png")) {
        std::cerr << "fail" << std::endl;
        return;
    }
    texture.setSmooth(false);

    sprite.setTexture(texture);
    initializeFrames();
    sprite.setTextureRect(frames[0]);
    sprite.setPosition(position);
    frameTime = 0.2f;
}


void PowerUpSprite::initializeFrames() {
    if (!broken) {

        frames.emplace_back(sf::IntRect(
                sf::Vector2i(8, 312),
                sf::Vector2i(32, 32)
        ));


        switch(type) {
            case PowerUpType::RINGS:
                frames.emplace_back(sf::IntRect(
                        sf::Vector2i(80, 367),
                        sf::Vector2i(16, 16)
                ));
                break;
            case PowerUpType::INVINCIBILITY:
                frames.emplace_back(sf::IntRect(
                        sf::Vector2i(56, 391),
                        sf::Vector2i(16, 16)
                ));
                break;
            case PowerUpType::SPEED:
                frames.emplace_back(sf::IntRect(
                        sf::Vector2i(8, 391),
                        sf::Vector2i(16, 16)
                ));
                break;
            case PowerUpType::SHIELD:
                frames.emplace_back(sf::IntRect(
                        sf::Vector2i(32, 391),
                        sf::Vector2i(16, 16)
                ));
                break;
            case PowerUpType::HEALTH:
                frames.emplace_back(sf::IntRect(
                        sf::Vector2i(80, 391),
                        sf::Vector2i(16, 16)
                ));
                break;
        }
    } else {

        frames.clear();
        frames.emplace_back(sf::IntRect(
                sf::Vector2i(48, 328),
                sf::Vector2i(32, 16)
        ));

    }
}


bool PowerUpSprite::checkCollision(const sf::FloatRect& bounds, bool isBallState) const {
    if (broken) {
        return false;
    }

    auto intersection = sprite.getGlobalBounds().findIntersection(bounds);
    if (!intersection) {
        return false;
    }


    if (!isBallState) {
        auto spriteBounds = sprite.getGlobalBounds();
        auto playerBottom = bounds.position.y + bounds.size.y;
        auto boxTop = spriteBounds.position.y;

        float penetrationDepth = playerBottom - boxTop;
        if (bounds.position.y < boxTop && penetrationDepth <= 16.0f) {
            return true;
        }
        return false;
    }

    return true;
}

sf::FloatRect PowerUpSprite::getCollisionBounds() const {
    return sprite.getGlobalBounds();
}

void PowerUpSprite::render(sf::RenderWindow& window) {
    sprite.setTextureRect(frames[0]);
    window.draw(sprite);

    if (frames.size() > 1) {
        sf::Vector2f originalPos = sprite.getPosition();
        sf::Vector2f iconPos = originalPos;
        iconPos.x += 8;
        iconPos.y += 6;

        sprite.setPosition(iconPos);
        sprite.setTextureRect(frames[1]);
        window.draw(sprite);

        sprite.setPosition(originalPos);
    }
}
