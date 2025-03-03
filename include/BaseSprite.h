#ifndef BASESPRITE_H
#define BASESPRITE_H

#include <SFML/Graphics.hpp>
#include <vector>

class BaseSprite {
public:
    BaseSprite(const sf::Vector2f& pos, bool animated = false);
    virtual ~BaseSprite() = default;

    virtual void update(float deltaTime);
    virtual void render(sf::RenderWindow& window);
    void setPosition(const sf::Vector2f& pos);
    void setOpacity(uint8_t alpha);

    const sf::Vector2f& getPosition() const { return position; }
    const sf::Sprite& getSprite() const { return sprite; }

protected:
    virtual void initializeFrames() {}

    sf::Vector2f position;
    sf::Texture texture;
    sf::Sprite sprite;
    std::vector<sf::IntRect> frames;
    size_t currentFrame{0};
    float frameTime{0.1f};
    float animationTimer{0.0f};
    bool isAnimated{false};
};

#endif