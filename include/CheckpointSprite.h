#ifndef CHECKPOINTSPRITE_H
#define CHECKPOINTSPRITE_H

#include "BaseSprite.h"

class CheckpointSprite final : public BaseSprite {
private:
    void initializeFrames() override;
    bool isActivated = false;
    bool hasPlayedAnimation = false;

public:
    explicit CheckpointSprite(const sf::Vector2f& pos);
    ~CheckpointSprite() override = default;
    void update(float deltaTime) override;
    void activate();
    bool isActive() const { return isActivated; }
    sf::FloatRect getCollisionBounds() const;

    void reset() {
        isActivated = false;
        hasPlayedAnimation = false;
        currentFrame = 0;
        animationTimer = 0.0f;
        sprite.setTextureRect(frames[0]);
    }

    static void createCheckpointGroup(std::vector<CheckpointSprite*>& sprites, const std::vector<sf::Vector2f>& positions);
};

#endif