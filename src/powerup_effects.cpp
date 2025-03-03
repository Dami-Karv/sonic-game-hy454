#include "PowerUpSprite.h"
#include "GameEngine.h"
#include "Player.h"
#include <iostream>
#include "../include/powerup_effects.h"

void PowerUpSprite::breakBox() {
    if (!broken) {
        broken = true;
        sf::Vector2f currentPos = sprite.getPosition();
        initializeFrames();
        sprite.setPosition(sf::Vector2f(currentPos.x, currentPos.y + 17.0f));
        if (engineRef) {
            engineRef->handlePowerUpEffect(type);
        }
    }
}