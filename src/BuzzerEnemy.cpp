#include "../include/BuzzerEnemy.h"
#include "SoundManager.h"
#include <iostream>
#include <cmath>

BuzzerEnemy::BuzzerEnemy(const sf::Vector2f& pos)
        : BaseSprite(pos, true), originalX(pos.x), moveDistance(0.0f), movingRight(true) {
    if (!texture.loadFromFile("./assets/enemies_sheet_fixed.png")) {
        std::cerr << "fail" << std::endl;
        return;
    }

    normalFrames[0] = sf::IntRect({8, 174}, {48, 32});
    normalFrames[1] = sf::IntRect({64, 174}, {48, 32});
    normalFrames[2] = sf::IntRect({8, 214}, {48, 24});
    normalFrames[3] = sf::IntRect({64, 214}, {48, 24});
    shootingFrame = sf::IntRect({72, 246}, {56, 56});
    projectileFrame = sf::IntRect({120, 150}, {16, 16});

    sprite.setTexture(texture);
    initializeFrames();
    sprite.setTextureRect(normalFrames[0]);
    sprite.setPosition(position);
    frameTime = 0.15f;

    sprite.setScale(sf::Vector2f(-1.f, 1.f));
    sprite.setOrigin(sf::Vector2f(sprite.getGlobalBounds().size.x, 0.f));
    SoundManager::getInstance().loadSound("badnik-death", "./assets/badnik-death.mp3");
}

void BuzzerEnemy::initializeFrames() {
    for(int i = 0; i < 4; ++i) {
        frames.push_back(normalFrames[i]);
    }
}

void BuzzerEnemy::update(float deltaTime) {
    if (!isActive) {
        if (freedAnimal) {
            freedAnimal->update(deltaTime);
            if (freedAnimal->shouldBeDeleted()) freedAnimal.reset();
        }
        return;
    }

    if (isShooting) {
        shootingTimer += deltaTime;
        if (shootingTimer >= SHOOTING_DURATION) {
            isShooting = false;
            shootingTimer = 0.0f;
            sprite.setTextureRect(normalFrames[currentFrame]);
        }
    }

    attackCooldown -= deltaTime;
    updateProjectiles(deltaTime);
    if (!isShooting) {
        updateMovement(deltaTime);
        updateAnimation(deltaTime);
    }
}

//function that updates the movement of the projectiles
void BuzzerEnemy::updateProjectiles(float deltaTime) {
    auto it = projectiles.begin();
    while (it != projectiles.end()) {
        if (!it->active || (collisionMap && collisionMap->checkCollision(
                sf::FloatRect(it->position, sf::Vector2f(16.f, 16.f)), 0))) {
            it = projectiles.erase(it);
        } else {
            it->position += it->velocity * deltaTime;
            ++it;
        }
    }
}

void BuzzerEnemy::updateMovement(float deltaTime) {
    moveDistance += (movingRight ? MOVE_SPEED : -MOVE_SPEED);
    if (std::abs(moveDistance) >= MAX_DISTANCE) {
        movingRight = !movingRight;
        sprite.setScale(sf::Vector2f(movingRight ? -1.f : 1.f, 1.f));
        sprite.setOrigin(movingRight ?
                         sf::Vector2f(sprite.getGlobalBounds().size.x, 0.f) : sf::Vector2f(0.f, 0.f));
    }
    position.x = originalX + moveDistance;
    sprite.setPosition(position);
}

void BuzzerEnemy::updateAnimation(float deltaTime) {
    if (isShooting) return;
    animationTimer += deltaTime;
    if (animationTimer >= frameTime) {
        animationTimer = 0;
        currentFrame = (currentFrame + 1) % 4;
        sprite.setTextureRect(normalFrames[currentFrame]);
    }
}

void BuzzerEnemy::render(sf::RenderWindow& window) const {
    if (!isActive) {
        if (freedAnimal) freedAnimal->render(window);
        return;
    }
    window.draw(sprite);

    sf::Sprite projectileSprite(texture);
    projectileSprite.setTextureRect(projectileFrame);
    for (const auto& proj : projectiles) {
        if (proj.active) {
            projectileSprite.setPosition(proj.position);
            window.draw(projectileSprite);
        }
    }
}

//function that makes the enemy shoot
void BuzzerEnemy::shoot(const sf::Vector2f& targetPos) {
    if (isShooting || attackCooldown > 0) return;

    isShooting = true;
    attackCooldown = ATTACK_COOLDOWN;
    shootingTimer = 0.0f;
    sprite.setTextureRect(shootingFrame);

    sf::Vector2f shootPos = position + sf::Vector2f(
            sprite.getGlobalBounds().size.x / 2,
            sprite.getGlobalBounds().size.y / 2
    );

    sf::Vector2f direction = targetPos - shootPos;
    float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    direction = direction / length;

    projectiles.push_back({shootPos, direction * Projectile::SPEED, true});
}

//function that returns the detection box of the enemy
sf::FloatRect BuzzerEnemy::getDetectionBox() const {
    return sf::FloatRect(
            sf::Vector2f(
                    position.x + (movingRight ? sprite.getGlobalBounds().size.x : -DETECTION_WIDTH),
                    position.y - (DETECTION_HEIGHT - sprite.getGlobalBounds().size.y) / 2
            ),
            sf::Vector2f(DETECTION_WIDTH, DETECTION_HEIGHT)
    );
}

//function that checks if the player is in range of the enemy
bool BuzzerEnemy::checkPlayerInRange(const sf::FloatRect& playerBounds) const {
    return getDetectionBox().findIntersection(playerBounds).has_value();
}

bool BuzzerEnemy::checkProjectileCollision(const sf::FloatRect& bounds) const {
    return std::any_of(projectiles.begin(), projectiles.end(),
                       [&bounds](const Projectile& proj) {
                           return proj.active && bounds.findIntersection(
                                   sf::FloatRect(proj.position, sf::Vector2f(16.f, 16.f))
                           ).has_value();
                       }
    );
}

//function that kills the enemy
void BuzzerEnemy::die() {
    if (isActive) {
        isActive = false;
        SoundManager::getInstance().playSound("badnik-death");
        freedAnimal = std::make_unique<AnimalSprite>(position, (std::rand() % 2) == 0);
        if (collisionMap) freedAnimal->setCollisionMap(collisionMap);
    }
}

//function that returns the collision bounds of the enemy
sf::FloatRect BuzzerEnemy::getCollisionBounds() const {
    sf::FloatRect bounds = sprite.getGlobalBounds();
    sf::Vector2f size(bounds.size.x * 0.7f, bounds.size.y * 0.8f);
    return sf::FloatRect(
            bounds.position + sf::Vector2f(
                    (bounds.size.x - size.x) * 0.5f,
                    (bounds.size.y - size.y) * 0.5f
            ),
            size
    );
}

//function that resets the enemy
void BuzzerEnemy::reset() {

    position.x = originalX;
    moveDistance = 0.0f;
    movingRight = true;
    isActive = true;
    sprite.setScale(sf::Vector2f(-1.f, 1.f));
    sprite.setOrigin(sf::Vector2f(sprite.getGlobalBounds().size.x, 0.f));
    sprite.setPosition(position);
    sprite.setTextureRect(normalFrames[0]);
    isShooting = false;
    shootingTimer = 0.0f;
    attackCooldown = 0.0f;
    projectiles.clear();
    currentFrame = 0;
    animationTimer = 0.0f;
    freedAnimal.reset();
}