#include "../include/Player.h"
#include "../include/FishEnemy.h"
#include "SoundManager.h"
#include <iostream>
#include <cmath>

Player::Player() : sprite(texture), animState(IDLE), animSwitch(true) {
    this->initPlayer();
    this->initPhysics();
    this->initAnimation();
    this->initSounds();
}


Player::~Player() = default;

void Player::loseRings() {

    int ringsToScatter = std::min(ringCount, 32);
    ringCount = 0;


}


void Player::initPlayer() {
    if (!texture.loadFromFile("./assets/sonic_sheet_fixed.png")) {
        std::cerr << "fail" << std::endl;
        return;
    }

    this->frame = sf::IntRect({ 27, 39 }, { 32, 40 });
    this->sprite.setTexture(texture);
    this->sprite.setTextureRect(frame);
    normalSize = getSpriteSize();
    auto& soundManager = SoundManager::getInstance();
    soundManager.loadSound("jump", "./assets/jump.mp3");
    soundManager.loadSound("bumper", "./assets/bumper.mp3");
    soundManager.loadSound("ring-collect", "./assets/ring-collect.mp3");
}

// Gets the collision bounds rectangle for the player
sf::FloatRect Player::getCollisionBounds() const {
    sf::Vector2f pos = sprite.getPosition();
    sf::Vector2f size = getSpriteSize();

    if (animState == PUSHING) {
        size.x = 24.0f;
    }


    float scaleX = sprite.getScale().x;
    float originX = (scaleX < 0) ? size.x : 0;

    return sf::FloatRect(sf::Vector2f(pos.x - originX, pos.y), size);
}


sf::Vector2f Player::getSpriteSize() const {
    auto rect = sprite.getTextureRect();
    auto scale = sprite.getScale();
    return sf::Vector2f(rect.size.x * std::abs(scale.x), rect.size.y * scale.y);
}


bool Player::isHighWall() const {
    if (!collisionMap) return false;

    sf::FloatRect bounds = getCollisionBounds();
    float directionX = (sprite.getScale().x > 0) ? 1.0f : -1.0f;

    float heightStep = bounds.size.y / 4.0f;
    int collisionCount = 0;
    float checkDistance = 2.0f;

    for (int i = 1; i <= 3; i++) {
        float checkY = bounds.position.y + (heightStep * i);

        sf::Vector2f checkPoint(
                bounds.position.x + (directionX > 0 ? bounds.size.x + checkDistance : -checkDistance),
                checkY
        );

        sf::FloatRect testRect(
                sf::Vector2f(checkPoint.x - 1.0f, checkPoint.y - 1.0f),
                sf::Vector2f(2.0f, 2.0f)
        );

        if (collisionMap->checkCollision(testRect, velocity.y)) {
            collisionCount++;
        }
    }

    return collisionCount >= 2;
}


void Player::updateMovement() {
    if (isGodMode) {
        handleGodModeMovement();
        return;
    }

    bool isPushingWall = false;

    if (isOnGround && !controlLock) {
        handleGroundMovement(isPushingWall);
    }
    else {
        handleAirMovement();
    }

    handleJumping();

    updateMovementAnimation(isPushingWall);


}

void Player::handleGodModeMovement() {
    const float godModeSpeed = 5.0f;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
        velocity.x = -godModeSpeed;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
        velocity.x = godModeSpeed;
    }
    else {
        velocity.x = 0;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) {
        velocity.y = -godModeSpeed;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) {
        velocity.y = godModeSpeed;
    }
    else {
        velocity.y = 0;
    }

    if (velocity.x != 0.0f) {
        updateSpriteDirection(velocity.x > 0);
    }
}

void Player::handleGroundMovement(bool& isPushingWall) {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
        handleRightMovement(isPushingWall);
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
        handleLeftMovement();
    }
    else {
        handleNoMovement();
    }

    if (std::abs(groundSpeed) < 0.1f && std::abs(velocity.y) < 0.1f && animState == IDLE) {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) {
            animState = LOOKING_UP;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) {
            animState = CURLING_DOWN;
        }
    }

    velocity.x = groundSpeed;
}

void Player::handleRightMovement(bool& isPushingWall) {
    if (isHighWall()) {
        handleWallPush(isPushingWall);
    }
    else {
        handleRegularRightMovement();
    }
}

void Player::bounceFromSpring() {
    const float SPRING_FORCE = -20.0f;
    const float HORIZONTAL_BOOST = 1.5f;


    if (std::abs(velocity.x) > 0.1f) {
        velocity.x *= HORIZONTAL_BOOST;
    } else if (std::abs(groundSpeed) > 0.1f) {

        velocity.x = groundSpeed * HORIZONTAL_BOOST;
    }


    const float MAX_HORIZONTAL_SPRING_SPEED = 12.0f;
    if (std::abs(velocity.x) > MAX_HORIZONTAL_SPRING_SPEED) {
        velocity.x = (velocity.x > 0) ? MAX_HORIZONTAL_SPRING_SPEED : -MAX_HORIZONTAL_SPRING_SPEED;
    }

    velocity.y = SPRING_FORCE;
    isJumping = true;
    isOnGround = false;
    animState = JUMPING;
    frame = sf::IntRect({27, 407}, {32, 32});
    sprite.setTextureRect(frame);


    SoundManager::getInstance().playSound("bumper");
}



void Player::handleWallPush(bool& isPushingWall) {
    isPushingWall = true;
    if (animState != PUSHING) {
        sf::FloatRect currentBounds = getCollisionBounds();
        animState = PUSHING;
        sf::FloatRect newBounds = getCollisionBounds();
        float adjustment = (newBounds.size.x - currentBounds.size.x);
        sprite.setPosition(sf::Vector2f(sprite.getPosition().x - adjustment, sprite.getPosition().y));
    }
    groundSpeed = 0.0f;
}

void Player::handleRegularRightMovement() {
    if (groundSpeed < -skidThreshold) {
        isSkidding = true;
        animState = SKIDDING;
        groundSpeed = std::min(0.f, groundSpeed + DECELERATION_SPEED * 0.8f);
    }
    else {
        isSkidding = false;
        if (groundSpeed < 0) {
            groundSpeed += DECELERATION_SPEED;
            if (groundSpeed >= 0) groundSpeed = 0.5f;
        }
        else if (groundSpeed < currentMaxSpeed) {
            groundSpeed += ACCELERATION_SPEED;
            if (groundSpeed >= currentMaxSpeed)
                groundSpeed = currentMaxSpeed;
        }

        if (!isSkidding && std::abs(groundSpeed) > 0.1f) {
            animState = (std::abs(groundSpeed) >= currentMaxSpeed * 0.8f) ? FULL_SPEED : MOVING_RIGHT;
        }
    }
}


void Player::handleLeftMovement() {
    if (groundSpeed > skidThreshold) {
        isSkidding = true;
        animState = SKIDDING;
        groundSpeed = std::max(0.f, groundSpeed - DECELERATION_SPEED * 0.8f);
    }
    else {
        isSkidding = false;
        if (groundSpeed > 0) {
            groundSpeed -= DECELERATION_SPEED;
            if (groundSpeed <= 0) groundSpeed = -0.5f;
        }
        else if (groundSpeed > -TOP_SPEED) {
            groundSpeed -= ACCELERATION_SPEED;
            if (groundSpeed <= -TOP_SPEED) groundSpeed = -TOP_SPEED;
        }

        if (!isSkidding && std::abs(groundSpeed) > 0.1f) {
            animState = (std::abs(groundSpeed) >= TOP_SPEED * 0.8f) ? FULL_SPEED : MOVING_LEFT;
        }
    }
}

void Player::handleNoMovement() {
    if (animState == PUSHING) {
        sf::FloatRect currentBounds = getCollisionBounds();
        animState = IDLE;
        sf::FloatRect newBounds = getCollisionBounds();
        float adjustment = (newBounds.size.x - currentBounds.size.x);
        sprite.setPosition(sf::Vector2f(sprite.getPosition().x - adjustment, sprite.getPosition().y));
    }

    isSkidding = false;
    groundSpeed = groundSpeed > 0 ? std::max(0.f, groundSpeed - FRICTION_SPEED)
                                  : std::min(0.f, groundSpeed + FRICTION_SPEED);

    if (std::abs(groundSpeed) < 0.1f && std::abs(velocity.y) < 0.1f) {
        animState = IDLE;
    }
}


void Player::handleAirMovement() {
    isSkidding = false;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
        velocity.x = std::max(velocity.x - AIR_ACCELERATION, -currentMaxSpeed);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
        velocity.x = std::min(velocity.x + AIR_ACCELERATION, currentMaxSpeed);
    }
}


void Player::handleJumping() {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space)) {
        if (!jumpButtonHeld && !isJumping && isOnGround) {
            startJump();
        }
    }
    else {
        jumpButtonHeld = false;
        if (isJumping && velocity.y < MIN_JUMP_SPEED) {
            velocity.y = MIN_JUMP_SPEED;
        }
    }
}

void Player::startJump() {
    isJumping = true;
    jumpButtonHeld = true;
    velocity.y = JUMP_FORCE;
    animState = JUMPING;
    frame = sf::IntRect({27, 407}, {32, 32});
    groundSpeed = velocity.x;

    SoundManager::getInstance().playSound("jump");
}

void Player::updateMovementAnimation(bool isPushingWall) {
    if (std::abs(groundSpeed) > 0.1f && !isPushingWall && !isJumping && !isSkidding) {
        if (std::abs(groundSpeed) >= TOP_SPEED * 0.8f) {
            animState = FULL_SPEED;
        }
        else {
            animState = (groundSpeed > 0) ? MOVING_RIGHT : MOVING_LEFT;
        }
    }

    if (velocity.x != 0.0f && !isSkidding && !isPushingWall) {
        updateSpriteDirection(velocity.x > 0);
    }
}

void Player::updateSpriteDirection(bool facingRight) {
    if (facingRight) {
        sprite.setScale(sf::Vector2f(1.f, 1.f));
        sprite.setOrigin(sf::Vector2f(0.f, 0.f));
    }
    else {
        sprite.setScale(sf::Vector2f(-1.f, 1.f));
        sprite.setOrigin(sf::Vector2f(sprite.getGlobalBounds().size.x, 0.f));
    }
}




void Player::updateAnimation() {
    static PLAYER_ANIMATION_STATES previousState = static_cast<PLAYER_ANIMATION_STATES>(this->animState);

    if (this->animState == IDLE) {
        if (previousState == CURLING_DOWN) {
            float oldBottom = sprite.getPosition().y + sprite.getGlobalBounds().size.y;
            this->frame = sf::IntRect({27, 39}, {32, 40});
            this->sprite.setTextureRect(frame);
            float newHeight = sprite.getGlobalBounds().size.y;
            sprite.setPosition(sf::Vector2f(sprite.getPosition().x, oldBottom - newHeight));
        }

        if (timer.getElapsedTime().asSeconds() >= 0.5f || this->getAnimationSwitch()) {
            if (idleClock.getElapsedTime().asSeconds() > 5.0f) {
                static int frameIndex = 0;
                switch (frameIndex) {
                    case 0: this->frame = sf::IntRect({59 + 54, 39}, {32, 40}); break;
                    case 1: this->frame = sf::IntRect({59 + 54 + 32 + 38, 39}, {32, 40}); break;
                    case 2: this->frame = sf::IntRect({59 + 54 + 2 * 32 + 2 * 38, 39}, {32, 40}); break;
                    case 3: this->frame = sf::IntRect({59 + 54 + 3 * 32 + 3 * 38, 39}, {32, 40}); break;
                }
                if (frameIndex < 3) frameIndex++;
                else if (frameIndex == 3) frameIndex = 2;
            } else {
                this->frame = sf::IntRect({27, 39}, {32, 40});
            }
            this->timer.restart();
            this->sprite.setTextureRect(this->frame);
        }
    }
    else if (this->animState == JUMPING) {
        if (timer.getElapsedTime().asSeconds() >= 0.05f || this->getAnimationSwitch()) {
            static int jumpFrameIndex = 0;
            switch (jumpFrameIndex % 5) {
                case 0: this->frame = sf::IntRect({27, 407}, {32, 32}); break;
                case 1: this->frame = sf::IntRect({97, 407}, {32, 32}); break;
                case 2: this->frame = sf::IntRect({167, 407}, {32, 32}); break;
                case 3: this->frame = sf::IntRect({237, 407}, {32, 32}); break;
                case 4: this->frame = sf::IntRect({307, 407}, {32, 32}); break;
            }
            jumpFrameIndex++;
            this->timer.restart();
            this->sprite.setTextureRect(this->frame);
        }
    }
    else if (this->animState == SKIDDING) {
        if (timer.getElapsedTime().asSeconds() >= 0.1f || this->getAnimationSwitch()) {
            static int frameIndex = 0;
            switch (frameIndex % 2) {
                case 0: this->frame = sf::IntRect({463, 131}, {32, 40}); break;
                case 1: this->frame = sf::IntRect({525, 131}, {40, 40}); break;
            }
            frameIndex++;
            this->timer.restart();
            this->sprite.setTextureRect(this->frame);
        }
        if (groundSpeed > 0) {
            this->sprite.setScale({-1.f, 1.f});
            this->sprite.setOrigin({this->sprite.getGlobalBounds().size.x, 0.f});
        } else {
            this->sprite.setScale({1.f, 1.f});
            this->sprite.setOrigin({0.f, 0.f});
        }
    }
    else if (this->animState == LOOKING_UP) {
        if (previousState != LOOKING_UP) {
            this->frame = sf::IntRect({409, 39}, {32, 40});
            this->sprite.setTextureRect(this->frame);
        }
    }
    else if (this->animState == CURLING_DOWN) {
        if (previousState != CURLING_DOWN) {
            float oldBottom = sprite.getPosition().y + sprite.getGlobalBounds().size.y;
            this->frame = sf::IntRect({491, 47}, {40, 32});
            this->sprite.setTextureRect(this->frame);
            float newHeight = sprite.getGlobalBounds().size.y;
            sprite.setPosition(sf::Vector2f(sprite.getPosition().x, oldBottom - newHeight));
        }
    }

    else if (this->animState == PUSHING) {
        if (previousState != PUSHING) {
            this->frame = sf::IntRect({388, 404}, {32, 40});
            this->sprite.setTextureRect(this->frame);
            this->timer.restart();
        }
    }

    else if (this->animState == MOVING_RIGHT || this->animState == MOVING_LEFT) {
        if (timer.getElapsedTime().asSeconds() >= 0.15f || this->getAnimationSwitch()) {
            static int frameIndex = 0;
            switch (frameIndex % 6) {
                case 0: this->frame = sf::IntRect({30, 131}, {24, 40}); break;
                case 1: this->frame = sf::IntRect({93, 129}, {40, 40}); break;
                case 2: this->frame = sf::IntRect({163, 130}, {31, 40}); break;
                case 3: this->frame = sf::IntRect({233, 131}, {40, 40}); break;
                case 4: this->frame = sf::IntRect({303, 129}, {40, 40}); break;
                case 5: this->frame = sf::IntRect({374, 130}, {40, 40}); break;
            }
            frameIndex++;
            this->timer.restart();
            this->sprite.setTextureRect(this->frame);
        }
    }
    else if (this->animState == FULL_SPEED) {
        if (timer.getElapsedTime().asSeconds() >= 0.1f || this->getAnimationSwitch()) {
            static int frameIndex = 0;
            switch (frameIndex % 4) {
                case 0: this->frame = sf::IntRect({23, 314}, {32, 40}); break;
                case 1: this->frame = sf::IntRect({93, 314}, {32, 40}); break;
                case 2: this->frame = sf::IntRect({163, 315}, {32, 39}); break;
                case 3: this->frame = sf::IntRect({233, 314}, {32, 40}); break;
            }
            frameIndex++;
            this->timer.restart();
            this->sprite.setTextureRect(this->frame);
        }
    }
    else this->timer.restart();

    previousState = static_cast<PLAYER_ANIMATION_STATES>(this->animState);
}



void Player::move(const float dx, float) {
    if (!isOnGround) {
        velocity.x += dx * AIR_ACCELERATION * 2.0f;
        if (std::abs(velocity.x) > TOP_SPEED) {
            velocity.x = TOP_SPEED * (velocity.x > 0 ? 1.0f : -1.0f);
        }
    }
}

void Player::setGodMode(bool enabled) {
    isGodMode = enabled;
    if (isGodMode) {
        gravity = 0.0f;
        gravityMax = 0.0f;
    } else {
        gravity = 0.5f;
        gravityMax = 10.0f;
    }
}


void Player::setPosition(const float x, const float y) {
    this->sprite.setPosition(sf::Vector2f(x, y));
}

// Applies physics including gravity collision detection and velocity changes
void Player::updatePhysics() {
    if (!collisionMap) return;

    sf::Vector2f pos = sprite.getPosition();
    sf::FloatRect bounds = getCollisionBounds();

    if (!isGodMode) {

        sf::FloatRect groundCheck = bounds;
        groundCheck.position.y += 2.0f;
        bool wasOnGround = isOnGround;
        isOnGround = false;

        for (float x = 0; x < bounds.size.x; x += 4.0f) {
            sf::FloatRect pointCheck = groundCheck;
            pointCheck.position.x = bounds.position.x + x;
            pointCheck.size.x = 1.0f;
            if (collisionMap->checkCollision(pointCheck, velocity.y)) {
                isOnGround = true;
                break;
            }
        }

        if (isOnGround && !wasOnGround) {
            isJumping = false;
            velocity.y = 0;
            animState = IDLE;
            frame = sf::IntRect({27, 39}, {32, 40});
            sprite.setTextureRect(frame);
        }

        if (!isOnGround) {
            velocity.y += gravity;
            if (velocity.y > gravityMax) velocity.y = gravityMax;
        }

        if (isOnGround) {
            velocity.x *= groundDrag;
            if (std::abs(velocity.x) < velocityMin) velocity.x = 0.0f;
        } else {
            velocity.x *= drag;
            if (velocity.y < 0 && velocity.y > -4) {
                velocity.x -= ((velocity.x / 0.125f) / 256.0f);
            }
        }

        const float STEP_SIZE = 2.0f;
        float remainingY = velocity.y;

        while (std::abs(remainingY) > 0.0f) {
            float moveY = remainingY;
            if (std::abs(moveY) > STEP_SIZE) {
                moveY = STEP_SIZE * (remainingY > 0 ? 1.0f : -1.0f);
            }

            sf::FloatRect nextBounds = bounds;
            nextBounds.position.y += moveY;
            bool collision = false;

            if (moveY < 0 && velocity.y < 0) {
                if (!collisionMap->checkCollision(nextBounds, velocity.y) ||
                    collisionMap->getTileType(collisionMap->worldToTile(nextBounds.position)) != 1) {
                    pos.y += moveY;
                    bounds = nextBounds;
                } else {
                    velocity.y = 0;
                    break;
                }
            } else {
                for (float x = 0; x < bounds.size.x; x += 4.0f) {
                    sf::FloatRect pointCheck = nextBounds;
                    pointCheck.position.x = nextBounds.position.x + x;
                    pointCheck.size.x = 1.0f;
                    if (collisionMap->checkCollision(pointCheck, velocity.y)) {
                        collision = true;
                        break;
                    }
                }

                if (!collision) {
                    pos.y += moveY;
                    bounds = nextBounds;
                } else {
                    velocity.y = 0;
                    break;
                }
            }
            remainingY -= moveY;
        }

        float remainingX = velocity.x;
        while (std::abs(remainingX) > 0.0f) {
            float moveX = remainingX;
            if (std::abs(moveX) > STEP_SIZE) {
                moveX = STEP_SIZE * (remainingX > 0 ? 1.0f : -1.0f);
            }

            sf::FloatRect nextBounds = bounds;
            nextBounds.position.x += moveX;
            bool collision = false;

            for (float y = 0; y < bounds.size.y; y += 4.0f) {
                sf::FloatRect pointCheck = nextBounds;
                pointCheck.position.y = nextBounds.position.y + y;
                pointCheck.size.y = 1.0f;
                if (collisionMap->checkCollision(pointCheck, velocity.y)) {
                    collision = true;
                    break;
                }
            }

            if (!collision) {
                if (isOnGround) {
                    sf::FloatRect slopeCheck = nextBounds;
                    slopeCheck.position.y += 8.0f;
                    bool foundGround = false;

                    for (float checkY = 0; checkY <= 8.0f; checkY += 1.0f) {
                        slopeCheck.position.y = nextBounds.position.y + checkY;
                        if (collisionMap->checkCollision(slopeCheck, velocity.y)) {
                            pos.y = slopeCheck.position.y - 1.0f;
                            foundGround = true;
                            break;
                        }
                    }
                    if (!foundGround) isOnGround = false;
                }
                pos.x += moveX;
                bounds = nextBounds;
            } else {
                if (std::abs(velocity.x) > 1.0f && !isJumping) {
                    sf::FloatRect stepBounds = nextBounds;
                    stepBounds.position.y -= 8.0f;
                    if (!collisionMap->checkCollision(stepBounds, velocity.y)) {
                        pos.x += moveX;
                        pos.y -= 8.0f;
                        bounds = stepBounds;
                    } else {
                        velocity.x = 0;
                        break;
                    }
                } else {
                    velocity.x = 0;
                    break;
                }
            }
            remainingX -= moveX;
        }

    } else {
        pos += velocity;
        velocity *= 0.98f;
        isOnGround = false;
        isJumping = false;
    }

    sprite.setPosition(pos);
}

void Player::initAnimation() {
    this->timer.restart();
    this->animSwitch = true;
}

bool Player::getAnimationSwitch() {
    const bool s = this->animSwitch;
    if (this->animSwitch) this->animSwitch = false;
    return s;
}


void Player::resetAnimationTimer() {
    this->timer.restart();
    this->animSwitch = true;
}

void Player::initPhysics() {
    currentMaxSpeed = TOP_SPEED;
    velocityMax = currentMaxSpeed;
    velocityMin = 0.1f;
    acc = ACCELERATION_SPEED;
    drag = 0.96f;
    gravity = 0.5f;
    gravityMax = 10.0f;
    groundDrag = 0.90f;
    groundSpeed = 0.0f;
    isJumping = false;
    isOnGround = false;
    isDead = false;
    controlLock = false;
    controlLockTimer = 0.0f;
}


sf::Vector2f Player::getPosition() const {
    return this->sprite.getPosition();
}

void Player::update() {
    static sf::Clock deltaClock;
    float deltaTime = deltaClock.restart().asSeconds();

    if (isDead) {
        return;
    }

    if (checkTeleportArea()) {
        return;
    }

    if (invincibilityTimer > 0) {
        invincibilityTimer -= deltaTime;
        if (invincibilityTimer <= 0) {
            invincibilityTimer = 0;
            isInvincible = false;
        }
    }

    if (speedBoostTimer > 0) {
        speedBoostTimer -= deltaTime;
        if (speedBoostTimer <= 0) {
            speedBoostTimer = 0;
            speedBoostMultiplier = 1.0f;
            currentMaxSpeed = TOP_SPEED;
        }
    }

    checkDeathConditions();

    if (isDead) {
        return;
    }

    if (isHurt || isInvincible) {
        updateHurtState();
        if (isHurt) {
            return;
        }
    }

    if (!isHurt) {
        updateMovement();
        updateAnimation();
        updatePhysics();
    }

}


void Player::render(sf::RenderWindow &window) const {
    window.draw(sprite);



    static sf::Clock coordClock;
    if (coordClock.getElapsedTime().asSeconds() >= 1.0f) {
        sf::Vector2f pos = sprite.getPosition();

        coordClock.restart();
    }
}





void Player::checkDeathConditions() {

    if (sprite.getPosition().y > 1280) {
        triggerDeath();
        return;
    }

    if (isHurt || isInvincible) {
        return;
    }

    if (!engineRef) {

        return;
    }

    auto playerBounds = getCollisionBounds();


    const auto& spikes = engineRef->GetSpikeSprites();
    for (const auto* spike : spikes) {
        if (!spike) continue;

        auto spikeBounds = spike->getCollisionBounds();
        auto intersection = playerBounds.findIntersection(spikeBounds);

        if (intersection) {


            if (ringCount > 0) {

                enterHurtState();
                return;
            } else {

                triggerDeath();
                return;
            }
        }
    }

}



void Player::checkSpikeCollisions(const std::vector<SpikeSprite*>& spikes) {
    if (isInvincible || isHurt) {

        return;
    }



    auto playerBounds = getCollisionBounds();

    for (const auto* spike : spikes) {
        if (spike && playerBounds.findIntersection(spike->getCollisionBounds())) {


            if (ringCount > 0) {
                enterHurtState();
            } else {

                isDead = true;
                if (engineRef) {
                    engineRef->SetCurrentState(GameState::GAME_OVER);
                }
            }
            return;
        }
    }
}

void Player::handleDamage() {
    if (hasShield) {
        hasShield = false;
        return;
    }

    if (isInvincible || isHurt) return;



    if (ringLossSound && ringLossSound->getStatus() != sf::Sound::Status::Playing) {
        ringLossSound->play();
    }

    if (ringCount > 0) {
        enterHurtState();
    } else {
        triggerDeath();
    }
}


void Player::triggerDeath() {
    if (!isDead) {
        if (deathSound && deathSound->getStatus() != sf::Sound::Status::Playing) {
            deathSound->play();
        }

        isDead = true;
        if (engineRef) {

            engineRef->handlePlayerDeath();
        }
    }
}




void Player::enterHurtState() {


    if (isHurt || isInvincible) {
        return;
    }

    int ringsToScatter = std::min(ringCount, 32);
    ringCount = 0;

    isHurt = true;
    isInvincible = true;
    hurtTimer = 0.0f;
    hurtClock.restart();

    if (engineRef) {
        engineRef->updateRingDisplay();
    }


    if (ringsToScatter > 0) {
        scatterRings(ringsToScatter);
    }


    velocity.x = (velocity.x != 0) ? -velocity.x : (sprite.getScale().x > 0 ? -5.0f : 5.0f);
    velocity.y = HURT_BOUNCE_VELOCITY_Y;

    controlLock = true;


    sprite.setColor(sf::Color(255, 100, 100, 255));
}



void Player::scatterRings(int ringsToScatter) {
    if (ringsToScatter <= 0) {
        return;
    }


    ringsToScatter = std::min(ringsToScatter, 32);

    if (!engineRef) {

        return;
    }


    const float INNER_RADIUS = 20.0f;
    const float OUTER_RADIUS = 40.0f;
    const int INNER_RING_COUNT = ringsToScatter / 2;

    sf::Vector2f playerPos = sprite.getPosition();
    int ringsCreated = 0;


    for (int i = 0; i < INNER_RING_COUNT; ++i) {
        float angle = (2.0f * M_PI * i) / INNER_RING_COUNT;
        float spawnX = playerPos.x + INNER_RADIUS * std::cos(angle);
        float spawnY = playerPos.y + INNER_RADIUS * std::sin(angle);

        float velX = 2.0f * std::cos(angle);
        float velY = -4.0f + (std::rand() % 2);

        engineRef->CreateScatteredRing(
                sf::Vector2f(spawnX, spawnY),
                sf::Vector2f(velX, velY)
        );
        ringsCreated++;
    }

    int outerRings = ringsToScatter - INNER_RING_COUNT;
    for (int i = 0; i < outerRings; ++i) {
        float angle = (2.0f * M_PI * i) / outerRings;
        float spawnX = playerPos.x + OUTER_RADIUS * std::cos(angle);
        float spawnY = playerPos.y + OUTER_RADIUS * std::sin(angle);

        float velX = 3.0f * std::cos(angle);
        float velY = -5.0f + (std::rand() % 3);

        engineRef->CreateScatteredRing(
                sf::Vector2f(spawnX, spawnY),
                sf::Vector2f(velX, velY)
        );
        ringsCreated++;
    }

}

void Player::resetAfterDeath() {
    velocity = sf::Vector2f(0.f, 0.f);
    groundSpeed = 0.0f;

    isDead = false;
    isHurt = false;
    isInvincible = false;
    isJumping = false;
    isOnGround = false;
    jumpButtonHeld = false;
    controlLock = false;
    isSkidding = false;
    ringCount = 0;

    animState = IDLE;
    frame = sf::IntRect({ 27, 39 }, { 32, 40 });
    sprite.setTextureRect(frame);


    sprite.setScale(sf::Vector2f(1.f, 1.f));
    sprite.setOrigin(sf::Vector2f(0.f, 0.f));
    sprite.setColor(sf::Color::White);


    gravity = 0.5f;
    gravityMax = 10.0f;


    timer.restart();
    idleClock.restart();
    hurtClock.restart();

    if (engineRef) {
        engineRef->updateRingDisplay();
    }

}



void Player::updateHurtState() {
    if (!isHurt && !isInvincible) {
        return;
    }

    float elapsedHurtTime = hurtClock.getElapsedTime().asSeconds();

    if (isHurt) {
        velocity.y = std::min(velocity.y + HURT_GRAVITY, gravityMax);
        updatePhysics();

        static const float HURT_DURATION = 1.0f;
        if (isOnGround || elapsedHurtTime >= HURT_DURATION) {
            velocity.x = 0.0f;
            velocity.y = 0.0f;
            gravity = 0.5f;
            isHurt = false;
            controlLock = false;
        }
    }

    if (isInvincible) {
        static const float INVINCIBILITY_DURATION = 2.0f;

        if (elapsedHurtTime >= INVINCIBILITY_DURATION) {
            isInvincible = false;
            sprite.setColor(sf::Color::White);
            hurtClock.restart();
        } else {
            static const float FLASH_FREQUENCY = 15.0f;
            float flashValue = std::abs(std::sin(elapsedHurtTime * FLASH_FREQUENCY));
            uint8_t alpha = static_cast<uint8_t>(flashValue * 255);

            sprite.setColor(sf::Color(255, 100, 100, alpha));
        }
    }
}




void Player::resetGame() {
    velocity = sf::Vector2f(0.f, 0.f);
    groundSpeed = 0.0f;

    isDead = false;
    isHurt = false;
    isInvincible = false;
    isJumping = false;
    isOnGround = false;
    jumpButtonHeld = false;
    controlLock = false;
    isSkidding = false;


    ringCount = 0;

    animState = IDLE;
    frame = sf::IntRect({ 27, 39 }, { 32, 40 });
    sprite.setTextureRect(frame);

    sprite.setScale(sf::Vector2f(1.f, 1.f));
    sprite.setOrigin(sf::Vector2f(0.f, 0.f));
    sprite.setColor(sf::Color::White);

    gravity = 0.5f;
    gravityMax = 10.0f;

    timer.restart();
    idleClock.restart();
    hurtClock.restart();

}