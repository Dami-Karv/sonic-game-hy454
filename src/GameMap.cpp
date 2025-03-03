#include "../include/GameMap.h"
#include <cmath>
#include <fstream>
#include <sstream>

GameMap::GameMap(const int tileWidth, const int tileHeight,
                 const int tileMargin, const int tileSpacing,
                 const std::string &tex_path, const std::string &csv_path)
        : sprite(texture), totalTilesX(0), totalTilesY(0) {
    this->tileWidth = tileWidth;
    this->tileHeight = tileHeight;
    this->tileMargin = tileMargin;
    this->tileSpacing = tileSpacing;
    this->loadMap(tex_path, csv_path);
}

GameMap::~GameMap() = default;

// Converts world coordinates to tile coordinates based on tile dimensions

sf::Vector2i GameMap::worldToTile(const sf::Vector2f& worldPos) const {
    return sf::Vector2i(
            static_cast<int>(worldPos.x / (tileWidth * 1.0f)),
            static_cast<int>(worldPos.y / (tileHeight * 1.0f))
    );
}

// Loads the map texture and CSV data, creating tile rectangles and map layout

void GameMap::loadMap(const std::string &texture_path, const std::string &csv_path) {
    if (!texture.loadFromFile(texture_path)) {
        return;
    }

    totalTilesX = texture.getSize().x / (tileWidth + tileSpacing);
    totalTilesY = texture.getSize().y / (tileHeight + tileSpacing);

    sprite.setTexture(texture);
    tileRects.clear();


    for (int tileY = 0; tileY < totalTilesY; ++tileY) {
        for (int tileX = 0; tileX < totalTilesX; ++tileX) {
            sf::IntRect rect(
                    {tileMargin + tileX * (tileWidth + tileSpacing),
                     tileMargin + tileY * (tileHeight + tileSpacing)},
                    {tileWidth, tileHeight}
            );
            tileRects.push_back(rect);
        }
    }


    std::ifstream file(csv_path);
    if (!file.is_open()) {
        return;
    }

    mapData.clear();
    std::string line;
    while (std::getline(file, line)) {
        std::vector<int> row;
        std::istringstream stream(line);
        std::string cell;

        while (std::getline(stream, cell, ',')) {
            try {
                row.push_back(std::stoi(cell));
            } catch (...) {
                row.push_back(0);
            }
        }

        if (!row.empty()) {
            mapData.push_back(row);
        }
    }
    file.close();
}

void GameMap::update() {}


// Performs collision detection between an object's bounds and the map tiles
// Also handles special collision cases like platform tops (type 1 tiles)
bool GameMap::checkCollision(const sf::FloatRect& bounds, float verticalVelocity) const {

    sf::Vector2i topLeft = worldToTile({bounds.position.x, bounds.position.y});
    sf::Vector2i bottomRight = worldToTile({bounds.position.x + bounds.size.x,
                                            bounds.position.y + bounds.size.y});

    int minX = std::min(topLeft.x, bottomRight.x);
    int maxX = std::max(topLeft.x, bottomRight.x);
    int minY = std::min(topLeft.y, bottomRight.y);
    int maxY = std::max(topLeft.y, bottomRight.y);


    for (int y = minY; y <= maxY; ++y) {
        for (int x = minX; x <= maxX; ++x) {
            if (x < 0 || x >= static_cast<int>(getMapWidth()) ||
                y < 0 || y >= static_cast<int>(getMapHeight())) {
                continue;
            }

            if (getTileType({x, y}) == 0) {

                sf::FloatRect tileRect(
                        sf::Vector2f(x * static_cast<float>(tileWidth),
                                     y * static_cast<float>(tileHeight)),
                        sf::Vector2f(static_cast<float>(tileWidth),
                                     static_cast<float>(tileHeight))
                );

                if (bounds.findIntersection(tileRect)) {
                    return true;
                }
            }
            else if (getTileType({x, y}) == 1) {
                float tileTopY = y * static_cast<float>(tileHeight);
                float objectBottomY = bounds.position.y + bounds.size.y;
                float objectTopY = bounds.position.y;


                float penetrationDepth = objectBottomY - tileTopY;

                if (verticalVelocity >= 0 &&
                    penetrationDepth >= 0 &&
                    penetrationDepth <= 16.0f &&
                    objectTopY < tileTopY)
                {
                    return true;
                }
            }
        }
    }
    return false;
}

// Renders the map tiles to the window with specified scale

void GameMap::render(sf::RenderWindow &window, float scale) {
    for (size_t row = 0; row < mapData.size(); ++row) {
        for (size_t col = 0; col < mapData[row].size(); ++col) {
            const int tileIndex = mapData[row][col];
            if (tileIndex < 0) continue;

            sprite.setTextureRect(tileRects[tileIndex]);
            sprite.setPosition({
                                       col * tileWidth * scale,
                                       row * tileHeight * scale
                               });
            sprite.setScale({scale, scale});
            window.draw(sprite);
        }
    }
}


// Shows solid tiles in red and platform tiles in green
void GameMap::renderCollisionDebug(sf::RenderWindow& window, float scale) const {
    static sf::RectangleShape collisionBox({tileWidth * scale, tileHeight * scale});
    collisionBox.setOutlineThickness(1.0f);
    sf::View view = window.getView();
    sf::Vector2f viewTopLeft = view.getCenter() - (view.getSize() / 2.f);

    int startX = std::max(0, static_cast<int>(viewTopLeft.x / (tileWidth * scale)));
    int startY = std::max(0, static_cast<int>(viewTopLeft.y / (tileHeight * scale)));
    int endX = std::min(static_cast<int>(getMapWidth()),
                        static_cast<int>((viewTopLeft.x + view.getSize().x) / (tileWidth * scale)) + 1);
    int endY = std::min(static_cast<int>(getMapHeight()),
                        static_cast<int>((viewTopLeft.y + view.getSize().y) / (tileHeight * scale)) + 1);
    for (int y = startY; y < endY; ++y) {
        for (int x = startX; x < endX; ++x) {
            switch(mapData[y][x]) {
                case 0:
                    collisionBox.setFillColor(sf::Color(255, 0, 0, 64));
                    collisionBox.setOutlineColor(sf::Color::Red);
                    break;
                case 1:
                    collisionBox.setFillColor(sf::Color(0, 255, 0, 64));
                    collisionBox.setOutlineColor(sf::Color::Green);
                    break;
                default:
                    continue;
            }
            collisionBox.setPosition(sf::Vector2f(x * tileWidth * scale, y * tileHeight * scale));
            window.draw(collisionBox);
        }
    }
}