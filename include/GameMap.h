#ifndef GAMEMAP_H
#define GAMEMAP_H
#include <SFML/Graphics.hpp>
#include <vector>

class GameMap {
    sf::Texture texture;
    sf::Sprite sprite;

    int tileWidth;
    int tileHeight;
    int tileMargin;
    int tileSpacing;
    int totalTilesX;
    int totalTilesY;

    std::vector<sf::IntRect> tileRects;
    std::vector<std::vector<int>> mapData;

    void loadMap(const std::string &, const std::string &);

public:
    GameMap(int tileWidth, int tileHeight, int tileMargin, int tileSpacing,
            const std::string &tex_path, const std::string &csv_path);
    ~GameMap();


    static void update();
    size_t getMapWidth() const { return !mapData.empty() ? mapData[0].size() : 0; }
    size_t getMapHeight() const { return mapData.size(); }



    bool isSolidTile(const sf::Vector2i& tilePos) const;
    bool checkCollision(const sf::FloatRect& bounds, float verticalVelocity) const;



    sf::Vector2i worldToTile(const sf::Vector2f& worldPos) const;


    void renderCollisionDebug(sf::RenderWindow& window, float scale) const;



    void render(sf::RenderWindow &window, float scale);

    int getTileType(const sf::Vector2i& tilePos) const {
        if (tilePos.x < 0 || tilePos.x >= static_cast<int>(getMapWidth()) ||
            tilePos.y < 0 || tilePos.y >= static_cast<int>(getMapHeight())) {
            return -1;
        }
        return mapData[tilePos.y][tilePos.x];
    }
};
#endif