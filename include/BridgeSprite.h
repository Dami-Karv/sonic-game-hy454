#ifndef BRIDGESPRITE_H
#define BRIDGESPRITE_H

#include "BaseSprite.h"

class BridgeSprite final : public BaseSprite {
private:
    void initializeFrames() override;

public:
    explicit BridgeSprite(const sf::Vector2f& pos);
    ~BridgeSprite() override = default;

    static void createBridgeGroup(std::vector<BridgeSprite*>& sprites, float startX, float startY, int count);
};

#endif