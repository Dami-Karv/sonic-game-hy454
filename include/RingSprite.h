#ifndef RINGSPRITE_H
#define RINGSPRITE_H

#include "BaseSprite.h"

class RingSprite final : public BaseSprite {
public:
    explicit RingSprite(const sf::Vector2f& pos);
    ~RingSprite() override = default;


    void update(float deltaTime) override;


    void collect();
    bool isActive() const { return !isCollectAnimationDone; }
    bool isCollected() const { return m_isCollected; }


    void setOpacity(uint8_t opacity) {
        sf::Color color = sprite.getColor();
        color.a = opacity;
        sprite.setColor(color);
    }


    sf::FloatRect getBounds() const { return sprite.getGlobalBounds(); }


    static void createRingGroup(std::vector<RingSprite*>& sprites,
                                float startX, float startY,
                                int count);




    void uncollect() {
        m_isCollected = false;
        isCollectAnimationDone = false;
        collectFrameIndex = 0;
        animationTimer = 0.0f;
        sprite.setTextureRect(frames[0]);  // Reset to initial frame
        sprite.setColor(sf::Color::White); // Reset any color changes
    }


private:

    void initializeFrames() override;
    void initializeCollectFrames();
    bool m_isCollected{false};
    bool isCollectAnimationDone{false};
    std::vector<sf::IntRect> collectFrames;
    int collectFrameIndex{0};
};




#endif