#ifndef SPRINGSPRITE_H
#define SPRINGSPRITE_H

#include "BaseSprite.h"
#include <SFML/Graphics.hpp>

class SpringSprite final : public BaseSprite {
public:
    static constexpr float SPRING_SPEED = 10.0f;
    static constexpr float VERTICAL_WIDTH = 33.0f;
    static constexpr float VERTICAL_HEIGHT = 17.0f;
    static constexpr float PULL_DISTANCE = 8.0f;

    explicit SpringSprite(const sf::Vector2f& pos);
    ~SpringSprite() override = default;

    void update(float deltaTime) override;
    void extend();
    bool canBounce() const { return m_canBounce; }

    // Returns the collision bounds of the spring
    sf::FloatRect getCollisionBounds() const {
        sf::FloatRect bounds = sprite.getGlobalBounds();
        float centerOffset = (VERTICAL_WIDTH - NORMAL_FRAME_SIZE.x) / 2.0f;
        return sf::FloatRect(
                sf::Vector2f(position.x - centerOffset, bounds.position.y),
                sf::Vector2f(VERTICAL_WIDTH, VERTICAL_HEIGHT)
        );
    }

    static void createSpringGroup(std::vector<SpringSprite*>& springs,
                                  const std::vector<sf::Vector2f>& positions);

private:
    void initializeFrames() override;

    static constexpr float EXTENSION_DURATION = 0.3f;
    static constexpr sf::Vector2i NORMAL_FRAME_POS{554, 313};
    static constexpr sf::Vector2i NORMAL_FRAME_SIZE{32, 16};
    static constexpr sf::Vector2i EXTENDED_FRAME_POS{554, 337};
    static constexpr sf::Vector2i EXTENDED_FRAME_SIZE{32, 32};

    bool m_extended{false};
    bool m_canBounce{true};
    float m_extensionTimer{0.0f};
    sf::IntRect m_normalFrame;
    sf::IntRect m_extendedFrame;
};

#endif