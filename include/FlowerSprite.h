#ifndef FLOORSPRITE_H
#define FLOORSPRITE_H

#include <SFML/Graphics.hpp>
#include <vector>

class FlowerSprite final {
private:
    sf::Texture texture;
    sf::Sprite sprite;
    std::vector<sf::IntRect> frames;
    float animationTimer = 0.0f;
    float frameTime = 0.3f;
    size_t currentFrame = 0;
    sf::Vector2f position;
    bool isMultiFrameFlower;

    void initializeFrames(bool isMultiFrame);

public:
    FlowerSprite(bool isMultiFrame, const sf::Vector2f& pos);
    ~FlowerSprite() = default;

    void update(float deltaTime);
    void render(sf::RenderWindow& window);
    void setPosition(const sf::Vector2f& pos);
};

#endif