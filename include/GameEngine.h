#ifndef GAMEENGINE_H
#define GAMEENGINE_H

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <memory>
#include <optional>
#include <map>

#include "GameState.h"
#include "GameMap.h"
#include "Player.h"
#include "GameStateManager.h"
#include "FlowerSprite.h"
#include "BridgeSprite.h"
#include "RingSprite.h"
#include "SpikeSprite.h"
#include "CheckpointSprite.h"
#include "BuzzerEnemy.h"
#include "MotobugEnemy.h"
#include "CrabmeatEnemy.h"
#include "FishEnemy.h"
#include "PowerUpSprite.h"
#include "PlatformSprite.h"
#include "SpringSprite.h"

class Player;

class GameEngine final {
public:
    GameEngine();
    ~GameEngine();


    bool running() const;
    void update();
    void render();
    void poll();
    void resetGame();


    static constexpr float LEVEL_END_X = 10000.0f;
    static constexpr float CAMERA_X_THRESHOLD = 5888.0f;
    static constexpr float CAMERA_Y_MAX_BEFORE_THRESHOLD = 1023.0f;
    static constexpr float CAMERA_Y_MAX_AFTER_THRESHOLD = 1280.0f;
    static constexpr int INITIAL_LIVES = 3;


    void SetCurrentState(GameState state) { currentState = state; }
    GameState GetCurrentState() const { return currentState; }

    void resetRings();
    void resetEnemies();
    void storeInitialEnemyPositions();

    void setGodMode(bool enabled);



    void updateLivesDisplay();
    void handlePlayerDeath();
    void respawnPlayer();
    void increasePlayerLives(int amount);
    Player* GetPlayer() { return player; }


    void updateRingDisplay();
    void updateScatteredRings(float deltaTime);
    void renderScatteredRings();
    void CreateScatteredRing(const sf::Vector2f& position, const sf::Vector2f& velocity);

    const std::vector<SpikeSprite*>& GetSpikeSprites() const { return spikeSprites; }
    void initFlowerSprites();
    void initPowerUpSprites();
    void handlePowerUpEffect(PowerUpSprite::PowerUpType type);

    std::vector<SpringSprite*> springSprites;



    class ScatteredRing {
    public:
        ScatteredRing(const sf::Vector2f& pos, const sf::Vector2f& vel);

        bool isActive() const { return active; }
        bool isOnFloor() const { return onFloor; }
        float getLifetime() const { return lifetime; }
        float getFloorTime() const { return floorTime; }
        const sf::Vector2f& getVelocity() const { return velocity; }
        RingSprite* getSprite() const { return sprite.get(); }


        void updateLifetime(float dt) { lifetime += dt; }
        void updateFloorTime(float dt) { floorTime += dt; }
        void setVelocity(const sf::Vector2f& vel) { velocity = vel; }
        void setActive(bool value) { active = value; }
        void setOnFloor(bool value) { onFloor = value; }

    private:
        std::unique_ptr<RingSprite> sprite;
        sf::Vector2f velocity;
        float lifetime{0.0f};
        float floorTime{0.0f};
        bool active{true};
        bool onFloor{false};

        friend class GameEngine;
    };

    struct EnemyInitialState {
        sf::Vector2f position;
        bool wasAlive;
    };

    std::map<BuzzerEnemy*, EnemyInitialState> buzzerInitialStates;
    std::map<MotobugEnemy*, EnemyInitialState> motobugInitialStates;
    std::map<CrabmeatEnemy*, EnemyInitialState> crabmeatInitialStates;
    std::map<FishEnemy*, EnemyInitialState> fishInitialStates;


private:

    sf::RenderWindow* window{nullptr};
    sf::VideoMode videoMode;
    sf::View view;
    sf::View bgr_view;
    sf::Vector2f viewBounds;
    sf::Vector2f minViewBounds;
    sf::Vector2f bgrViewBounds;
    sf::Vector2f minBgrViewBounds;


    GameState currentState{GameState::INTRO};
    bool isPaused{false};
    uint64_t pauseTime{0};
    bool isGodMode{false};
    bool isGridMapVisible{false};

    GameMap* map{nullptr};
    GameMap* bgr{nullptr};
    GameMap* collision{nullptr};
    Player* player{nullptr};
    GameStateManager* stateManager{nullptr};

    sf::Music bgMusic;
    float musicVolume{50.0f};
    bool isMusicMuted{true};

    sf::Font gameFont;
    sf::Font introFont;
    sf::Text* ringCountText{nullptr};
    sf::Text* livesCountText{nullptr};
    int displayedRingCount{0};
    int currentLives{INITIAL_LIVES};

    std::vector<FlowerSprite*> flowerSprites;
    std::vector<BridgeSprite*> bridgeSprites;
    std::vector<RingSprite*> ringSprites;
    std::vector<SpikeSprite*> spikeSprites;
    std::vector<CheckpointSprite*> checkpointSprites;
    std::vector<BuzzerEnemy*> buzzerEnemies;
    std::vector<MotobugEnemy*> motobugEnemies;
    std::vector<CrabmeatEnemy*> crabmeatEnemies;
    std::vector<FishEnemy*> fishEnemies;
    std::vector<PowerUpSprite*> powerUpSprites;
    std::vector<PlatformSprite*> platformSprites;
    std::vector<ScatteredRing> scatteredRings;

    std::optional<sf::Vector2f> lastCheckpoint;
    void handleCheckpointActivation(const sf::Vector2f& checkpointPos);
    sf::Vector2f getSpawnPosition() const;

    void initGameElements();
    void initText();
    void initLifeDisplay();

    void updateGameState();
    void constrainView();
    void constrainBackgroundView();


    void renderPlayingState();
    void renderPausedState();


    void cleanup();
    void cleanupText();
};

#endif