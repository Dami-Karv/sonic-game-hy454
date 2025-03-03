#include "../include/CrabmeatEnemy.h"
#include "../include/SoundManager.h"
#include <iostream>
#include <cmath>

CrabmeatEnemy::CrabmeatEnemy(const sf::Vector2f& pos)
        : BaseSprite(pos, true)
        , originalX(pos.x)
        , isActive(true)
        , movingRight(true)
{
    if (!texture.loadFromFile("./assets/enemies_sheet_fixed.png")) {
        std::cerr << "fail" << std::endl;
        return;
    }

    sprite.setTexture(texture);
    initializeFrames();
    sprite.setTextureRect(frames[0]);
    sprite.setPosition(position);
    frameTime = 0.2f;

    sprite.setScale(sf::Vector2f(1.f, 1.f));
    sprite.setOrigin(sf::Vector2f(0.f, 0.f));


    SoundManager::getInstance().loadSound("badnik-death", "./assets/badnik-death.mp3");
}

void CrabmeatEnemy::initializeFrames() {

    walkFrames = {
            sf::IntRect(sf::Vector2i(8, 29), sf::Vector2i(48, 32)),
            sf::IntRect(sf::Vector2i(64, 29), sf::Vector2i(48, 32)),
            sf::IntRect(sf::Vector2i(120, 25), sf::Vector2i(48, 40)),
            sf::IntRect(sf::Vector2i(176, 25), sf::Vector2i(48, 40)),
            sf::IntRect(sf::Vector2i(232, 29), sf::Vector2i(48, 32))
    };

    attackFrames = {
            sf::IntRect(sf::Vector2i(120, 25), sf::Vector2i(48, 40)),
    };

    frames = walkFrames;
}

void CrabmeatEnemy::update(float deltaTime) {
    if (!isActive) {
        if (freedAnimal) {
            freedAnimal->update(deltaTime);
            if (freedAnimal->shouldBeDeleted()) {
                freedAnimal.reset();
            }
        }
        return;
    }

    if (pauseTimer > 0) {
        pauseTimer -= deltaTime;
        return;
    }


    if (attackCooldown > 0) {
        attackCooldown -= deltaTime;
    }

    if (isShooting) {
        shootingTimer += deltaTime;
        if (shootingTimer >= SHOOTING_DURATION) {
            isShooting = false;
            shootingTimer = 0.0f;
            frames = walkFrames;
        }
    }


    updateProjectiles(deltaTime);

    if (!isShooting) {
        updatePatrolMovement(deltaTime);
    }

    updateAnimation(deltaTime);
}


void CrabmeatEnemy::updateProjectiles(float deltaTime) {
    auto it = projectiles.begin();
    while (it != projectiles.end()) {

        it->velocity.y += Projectile::GRAVITY * deltaTime;

        it->position += it->velocity * deltaTime;

        if (collisionMap && collisionMap->checkCollision(
                sf::FloatRect(it->position, sf::Vector2f(16.f, 16.f)), 0)) {
            it = projectiles.erase(it);
        }

        else if (std::abs(it->position.x - position.x) > 300.0f ||
                 std::abs(it->position.y - position.y) > 300.0f) {
            it = projectiles.erase(it);
        }
        else {
            ++it;
        }
    }
}

void CrabmeatEnemy::updateAnimation(float deltaTime) {
    animationTimer += deltaTime;
    if (animationTimer >= FRAME_TIME) {
        animationTimer = 0;
        currentFrame = (currentFrame + 1) % frames.size();
        sprite.setTextureRect(frames[currentFrame]);
    }
}

// Check if the player is in range of the enemy
bool CrabmeatEnemy::checkPlayerInRange(const sf::FloatRect& playerBounds) const {

    sf::Vector2f enemyCenter = position + sf::Vector2f(sprite.getGlobalBounds().size.x / 2,
                                                       sprite.getGlobalBounds().size.y / 2);
    sf::Vector2f playerCenter = sf::Vector2f(playerBounds.position.x + playerBounds.size.x / 2,
                                             playerBounds.position.y + playerBounds.size.y / 2);

    float distance = std::sqrt(std::pow(playerCenter.x - enemyCenter.x, 2) +
                               std::pow(playerCenter.y - enemyCenter.y, 2));
    return distance <= DETECTION_RANGE;
}


bool CrabmeatEnemy::checkProjectileCollision(const sf::FloatRect& bounds) const {
    return std::any_of(projectiles.begin(), projectiles.end(),
                       [&bounds](const Projectile& proj) {
                           return proj.active && bounds.findIntersection(
                                   sf::FloatRect(proj.position, sf::Vector2f(16.f, 16.f))
                           ).has_value();
                       });
}

void CrabmeatEnemy::shoot() {
    if (isShooting || attackCooldown > 0) return;

    isShooting = true;
    attackCooldown = ATTACK_COOLDOWN;
    shootingTimer = 0.0f;
    frames = attackFrames;


    sf::Vector2f shootPos = position + sf::Vector2f(
            sprite.getGlobalBounds().size.x / 2,
            sprite.getGlobalBounds().size.y / 2
    );

    float angleLeft = -60.0f;
    float radLeft = angleLeft * 3.14159f / 180.0f;
    projectiles.push_back({
                                  shootPos,
                                  sf::Vector2f(
                                          Projectile::INITIAL_SPEED * std::cos(radLeft),
                                          Projectile::INITIAL_SPEED * std::sin(radLeft)
                                  ),
                                  true
                          });


    float angleRight = -120.0f;
    float radRight = angleRight * 3.14159f / 180.0f;
    projectiles.push_back({
                                  shootPos,
                                  sf::Vector2f(
                                          Projectile::INITIAL_SPEED * std::cos(radRight),
                                          Projectile::INITIAL_SPEED * std::sin(radRight)
                                  ),
                                  true
                          });
}

// Update the enemy's patrol movement
void CrabmeatEnemy::updatePatrolMovement(float deltaTime) {
    const float MOVE_SPEED = 60.0f;
    float movement = MOVE_SPEED * deltaTime;

    sf::Vector2f prevPos = position;


    sf::Vector2f futurePos = position;
    if (movingRight) {
        futurePos.x += movement;
    } else {
        futurePos.x -= movement;
    }

    sf::FloatRect futureBounds = getCollisionBounds();
    futureBounds.position = futurePos;

    float groundCheckOffset = movingRight ? futureBounds.size.x + 5.0f : -5.0f;
    sf::FloatRect groundCheckRect(
            sf::Vector2f(futureBounds.position.x + groundCheckOffset, futureBounds.position.y + futureBounds.size.y),
            sf::Vector2f(2.0f, 32.0f)
    );

    bool shouldTurn = false;

    if (collisionMap && collisionMap->checkCollision(futureBounds, 0)) {
        shouldTurn = true;
    }

    if (!shouldTurn && collisionMap && !collisionMap->checkCollision(groundCheckRect, 0)) {

        groundCheckRect.position.x += (movingRight ? -10.0f : 10.0f);
        if (!collisionMap->checkCollision(groundCheckRect, 0)) {
            shouldTurn = true;
        }
    }

    if (shouldTurn && pauseTimer <= 0) {

        movingRight = !movingRight;
        position = prevPos;
        pauseTimer = TURN_PAUSE_DURATION;


        if (movingRight) {
            sprite.setScale(sf::Vector2f(-1.f, 1.f));
            sprite.setOrigin(sf::Vector2f(sprite.getGlobalBounds().size.x, 0.f));
        } else {
            sprite.setScale(sf::Vector2f(1.f, 1.f));
            sprite.setOrigin(sf::Vector2f(0.f, 0.f));
        }
    } else if (!shouldTurn) {
        position = futurePos;
    }

    sprite.setPosition(position);
}

void CrabmeatEnemy::die() {
    if (isActive) {
        isActive = false;
        SoundManager::getInstance().playSound("badnik-death");

        freedAnimal = std::make_unique<AnimalSprite>(position, true);
        if (freedAnimal && collisionMap) {
            freedAnimal->setCollisionMap(collisionMap);
        }
    }
}

// Render the enemy
void CrabmeatEnemy::render(sf::RenderWindow& window) const {
    if (isActive) {
        window.draw(sprite);

        sf::Sprite projectileSprite(texture);
        projectileSprite.setTextureRect(projectileFrame);
        for (const auto& proj : projectiles) {
            if (proj.active) {
                projectileSprite.setPosition(proj.position);
                window.draw(projectileSprite);
            }
        }
    } else if (freedAnimal) {
        freedAnimal->render(window);
    }
}
bool CrabmeatEnemy::isAlive() const {
    return isActive;
}


sf::FloatRect CrabmeatEnemy::getCollisionBounds() const {
    sf::FloatRect bounds = sprite.getGlobalBounds();
    sf::Vector2f size = bounds.size;
    sf::Vector2f pos = bounds.position;

    size.x *= 0.7f;
    size.y *= 0.8f;
    pos.x += size.x * 0.15f;
    pos.y += size.y * 0.1f;

    return sf::FloatRect(pos, size);
}

void CrabmeatEnemy::reset() {

    position.x = originalX;
    isActive = true;
    movingRight = true;

    sprite.setScale(sf::Vector2f(1.f, 1.f));
    sprite.setOrigin(sf::Vector2f(0.f, 0.f));
    sprite.setPosition(position);
    sprite.setTextureRect(frames[0]);


    float movementTimer = 0.0f;
    pauseTimer = 0.0f;
    animationTimer = 0.0f;
    currentFrame = 0;


    freedAnimal.reset();

    isShooting = false;
    shootingTimer = 0.0f;
    attackCooldown = 0.0f;
    projectiles.clear();
    frames = walkFrames;
}

