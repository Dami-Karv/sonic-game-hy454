#ifndef PLATFORMSPRITE_H
#define PLATFORMSPRITE_H

#include "BaseSprite.h"

class PlatformSprite final : public BaseSprite {
public:
    explicit PlatformSprite(const sf::Vector2f& pos);
    ~PlatformSprite() override = default;

    static void createPlatformGroup(std::vector<PlatformSprite*>& platforms,
                                    const std::vector<sf::Vector2f>& positions);

private:
    void initializeFrames() override;
    static constexpr sf::Vector2i SPRITE_POS{220, 25};
    static constexpr sf::Vector2i SPRITE_SIZE{60, 27};
};

#endif