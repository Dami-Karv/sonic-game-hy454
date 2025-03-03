#ifndef SPIKESPRITE_H
#define SPIKESPRITE_H

#include "BaseSprite.h"

class SpikeSprite final : public BaseSprite {
private:
    void initializeFrames() override;


public:
    explicit SpikeSprite(const sf::Vector2f& pos);
    ~SpikeSprite() override = default;

    sf::FloatRect getCollisionBounds() const;
    static void createSpikeGroup(std::vector<SpikeSprite*>& sprites, const std::vector<sf::Vector2f>& positions);
};

#endif