#include "../include/GameEngine.h"
#include "SpikeSprite.h"
#include <algorithm>
#include <iostream>

#define TILE_SIZE 256.0f
#define PARALLAX_FACTOR 0.45f
#define BG_SCALE 4.0f


GameEngine::GameEngine() : window(nullptr), videoMode(), currentState(GameState::INTRO),
                           isPaused(false), pauseTime(0), map(nullptr), bgr(nullptr), collision(nullptr),
                           player(nullptr), musicVolume(50.0f), isMusicMuted(false), stateManager(nullptr),
                           isGodMode(false), ringCountText(nullptr) {
    try {
        std::srand(static_cast<unsigned>(std::time(nullptr)));

        initGameElements();
        storeInitialEnemyPositions();
        if (!window) throw std::runtime_error("fail");
        stateManager = new GameStateManager(window, bgMusic, musicVolume, isMusicMuted, isGodMode, isGridMapVisible);


        stateManager->setEngineReference(this);

    }
    catch (const std::exception& e) {
        std::cerr << "fail " << e.what() << std::endl;
        cleanup();
        throw;
    }


}

GameEngine::~GameEngine() {
    cleanup();
}

void GameEngine::cleanup() {
    for (auto* flower : flowerSprites) delete flower;
    flowerSprites.clear();

    for (auto* bridge : bridgeSprites) delete bridge;
    bridgeSprites.clear();

    for (auto* checkpoint : checkpointSprites) delete checkpoint;
    checkpointSprites.clear();

    for (auto* buzzer : buzzerEnemies) delete buzzer;
    buzzerEnemies.clear();

    for (auto* ring : ringSprites) delete ring;
    ringSprites.clear();

    for (auto* crabmeat : crabmeatEnemies) delete crabmeat;
    crabmeatEnemies.clear();

    for (auto* motobug : motobugEnemies) delete motobug;
    motobugEnemies.clear();

    for (auto* spike : spikeSprites) delete spike;
    spikeSprites.clear();

    for (auto* fish : fishEnemies) delete fish;
    fishEnemies.clear();

    for (auto* powerUp : powerUpSprites) {
        delete powerUp;
    }
    powerUpSprites.clear();

    for (auto* spring : springSprites) {
        delete spring;
    }
    springSprites.clear();

    for (auto* powerUp : powerUpSprites) {
        delete powerUp;
    }
    powerUpSprites.clear();


    scatteredRings.clear();


    bgMusic.stop();
    delete window;
    delete map;
    delete bgr;
    delete collision;
    delete player;
    delete stateManager;

    window = nullptr;
    map = nullptr;
    bgr = nullptr;
    collision = nullptr;
    player = nullptr;
    stateManager = nullptr;
}

//function to initialize the game elements
void GameEngine::initGameElements() {
    videoMode = sf::VideoMode(sf::Vector2u(800, 600));
    window = new sf::RenderWindow(videoMode, "HY454 - Sonic", sf::Style::Titlebar | sf::Style::Close);
    window->setFramerateLimit(60);

    view.setSize(sf::Vector2f{1.4f * TILE_SIZE, 1.0f * TILE_SIZE});
    view.setCenter(sf::Vector2f{view.getSize().x / 2.0f, 3.0f * view.getSize().y});

    bgr_view = view;
    bgr_view.setSize(sf::Vector2f{view.getSize().x * BG_SCALE, view.getSize().y * BG_SCALE});
    bgr_view.setCenter(sf::Vector2f{BG_SCALE * view.getCenter().x, BG_SCALE * view.getSize().y / 2.0f});

    bgr = new GameMap(64, 64, 0, 0, "./assets/background_foreground64.png", "./assets/background.csv");
    map = new GameMap(256, 256, 8, 8, "./assets/Map_Tilesheet.png", "./assets/Map.csv");
    collision = new GameMap(2, 2, 0, 0, "./assets/grid_tile_set.png", "./assets/basic_gridmap.csv");

    auto mapSizeX = map->getMapWidth() * 256.0f;
    auto mapSizeY = map->getMapHeight() * 256.0f;
    auto bgrMapSizeX = bgr->getMapWidth() * 64.0f * BG_SCALE;
    auto bgrMapSizeY = bgr->getMapHeight() * 64.0f * BG_SCALE;

    minViewBounds = sf::Vector2f{view.getSize().x / 2.0f, view.getSize().y / 2.0f};
    viewBounds = sf::Vector2f{mapSizeX - view.getSize().x / 2.0f, mapSizeY - view.getSize().y / 2.0f};
    minBgrViewBounds = sf::Vector2f{bgr_view.getSize().x / 2.0f, bgr_view.getSize().y / 2.0f};
    bgrViewBounds = sf::Vector2f{bgrMapSizeX - bgr_view.getSize().x / 2.0f, bgrMapSizeY - bgr_view.getSize().y / 2.0f};

    player = new Player();
    player->setEngineRef(this);
    player->setPosition(179.2f, 919.0f);
    player->setCollisionMap(collision);

    if (!bgMusic.openFromFile("./assets/greenhill.mp3")) {
        throw std::runtime_error("fail");
    }
    bgMusic.setLooping(true);
    bgMusic.setVolume(musicVolume);

    initFlowerSprites();

    initPowerUpSprites();


    BridgeSprite::createBridgeGroup(bridgeSprites, 272 * 4, 224 * 4, 12);
    BridgeSprite::createBridgeGroup(bridgeSprites, 656 * 4, 196 * 4, 12);
    BridgeSprite::createBridgeGroup(bridgeSprites, 2000 * 4, 196 * 4, 12);

    RingSprite::createRingGroup(ringSprites, 79 * 4, 216 * 4, 3);

    RingSprite::createRingGroup(ringSprites, 281 * 4, 198 * 4, 6);
    RingSprite::createRingGroup(ringSprites, 622 * 4, 174 * 4, 2);
    RingSprite::createRingGroup(ringSprites, 735 * 4, 176 * 4, 2);
    RingSprite::createRingGroup(ringSprites, 1108 * 4, 201 * 4, 3);
    RingSprite::createRingGroup(ringSprites, 1157 * 4, 201 * 4, 3);
    RingSprite::createRingGroup(ringSprites, 1617 * 4, 217 * 4, 5);
    RingSprite::createRingGroup(ringSprites, 2006 * 4, 282 * 4, 6);

    ringSprites.push_back(new RingSprite(sf::Vector2f(3400, 686)));
    ringSprites.push_back(new RingSprite(sf::Vector2f(3432, 673)));
    ringSprites.push_back(new RingSprite(sf::Vector2f(3466, 655)));
    ringSprites.push_back(new RingSprite(sf::Vector2f(3500, 639)));


    ringSprites.push_back(new RingSprite(sf::Vector2f(3858, 509)));
    ringSprites.push_back(new RingSprite(sf::Vector2f(3895, 513)));
    ringSprites.push_back(new RingSprite(sf::Vector2f(3934, 512)));


    ringSprites.push_back(new RingSprite(sf::Vector2f(4692, 566)));
    ringSprites.push_back(new RingSprite(sf::Vector2f(4728, 577)));
    ringSprites.push_back(new RingSprite(sf::Vector2f(4762, 593)));
    ringSprites.push_back(new RingSprite(sf::Vector2f(4795, 609)));
    ringSprites.push_back(new RingSprite(sf::Vector2f(4834, 617)));

    ringSprites.push_back(new RingSprite(sf::Vector2f(1656 , 895)));
    ringSprites.push_back(new RingSprite(sf::Vector2f(1688 , 911)));
    ringSprites.push_back(new RingSprite(sf::Vector2f(1720 , 925)));
    ringSprites.push_back(new RingSprite(sf::Vector2f(1755 , 936)));
    ringSprites.push_back(new RingSprite(sf::Vector2f(1795 , 939)));
    ringSprites.push_back(new RingSprite(sf::Vector2f(1839 , 935)));
    ringSprites.push_back(new RingSprite(sf::Vector2f(1875 , 906)));

    ringSprites.push_back(new RingSprite(sf::Vector2f(5852, 429)));
    ringSprites.push_back(new RingSprite(sf::Vector2f(5892, 429)));
    ringSprites.push_back(new RingSprite(sf::Vector2f(5932, 419)));
    ringSprites.push_back(new RingSprite(sf::Vector2f(5971, 404)));
    ringSprites.push_back(new RingSprite(sf::Vector2f(6012, 385)));
    ringSprites.push_back(new RingSprite(sf::Vector2f(6046, 369)));


    ringSprites.push_back(new RingSprite(sf::Vector2f(6104, 364)));
    ringSprites.push_back(new RingSprite(sf::Vector2f(6148, 364)));
    ringSprites.push_back(new RingSprite(sf::Vector2f(6196, 366)));
    ringSprites.push_back(new RingSprite(sf::Vector2f(6244, 368)));
    ringSprites.push_back(new RingSprite(sf::Vector2f(6292, 370)));
    ringSprites.push_back(new RingSprite(sf::Vector2f(6340, 634)));


    ringSprites.push_back(new RingSprite(sf::Vector2f(6388, 635)));
    ringSprites.push_back(new RingSprite(sf::Vector2f(6436, 635)));
    ringSprites.push_back(new RingSprite(sf::Vector2f(6484, 370)));
    ringSprites.push_back(new RingSprite(sf::Vector2f(6532, 371)));
    ringSprites.push_back(new RingSprite(sf::Vector2f(6580, 366)));
    ringSprites.push_back(new RingSprite(sf::Vector2f(6628, 364)));


    ringSprites.push_back(new RingSprite(sf::Vector2f(9018, 1133)));
    ringSprites.push_back(new RingSprite(sf::Vector2f(9055, 1142)));
    ringSprites.push_back(new RingSprite(sf::Vector2f(9086, 1157)));
    ringSprites.push_back(new RingSprite(sf::Vector2f(9117, 1193)));
    ringSprites.push_back(new RingSprite(sf::Vector2f(9150, 1190)));
    ringSprites.push_back(new RingSprite(sf::Vector2f(9188, 1198)));
    ringSprites.push_back(new RingSprite(sf::Vector2f(9228, 1198)));
    ringSprites.push_back(new RingSprite(sf::Vector2f(9266, 1198)));
    ringSprites.push_back(new RingSprite(sf::Vector2f(9305, 1198)));
    ringSprites.push_back(new RingSprite(sf::Vector2f(9342, 1198)));

    std::vector<sf::Vector2f> spikePositions = {
            sf::Vector2f(835 * 4, 216 * 4),
            sf::Vector2f(847 * 4, 218 * 4),
            sf::Vector2f(860 * 4, 218 * 4),
            sf::Vector2f(881 * 4, 216 * 4),
            sf::Vector2f(977 * 4, 233 * 4),
            sf::Vector2f(997 * 4, 234 * 4),
            sf::Vector2f(1110 * 4, 218 * 4),
            sf::Vector2f(1136 * 4, 216 * 4),
            sf::Vector2f(1160 * 4, 217 * 4),
            sf::Vector2f(1325 * 4, 201 * 4),
            sf::Vector2f(1338 * 4, 200 * 4),
            sf::Vector2f(1351 * 4, 200 * 4),
            sf::Vector2f(1923 * 4, 297 * 4),
            sf::Vector2f(2051 * 4, 296 * 4)
    };
    SpikeSprite::createSpikeGroup(spikeSprites, spikePositions);

    std::vector<sf::Vector2f> checkpointPositions = {
            sf::Vector2f(1626 * 4, 210 * 4),
            sf::Vector2f(1140 * 4, 128 * 4)
    };

    CheckpointSprite::createCheckpointGroup(checkpointSprites, checkpointPositions);

    std::vector<sf::Vector2f> buzzerPositions = {
            sf::Vector2f(264 * 4, 195 * 4),
            sf::Vector2f(845 * 4, 142 * 4),
            sf::Vector2f(882 * 4, 146 * 4),
            sf::Vector2f(1092 * 4, 117 * 4),
            sf::Vector2f(1896 * 4, 190 * 4),
            sf::Vector2f(1952 * 4, 164 * 4),
            sf::Vector2f(2291 * 4, 218 * 4)
    };

    std::vector<sf::Vector2f> platformPositions = {
            sf::Vector2f(4546.0f, 479.0f),
            sf::Vector2f(4738.0f, 415.0f),
            sf::Vector2f(4994.0f, 383.0f),
            sf::Vector2f(5122.0f, 415.0f),
            sf::Vector2f(5250.0f, 447.0f),
            sf::Vector2f(5630.0f, 434.0f)
    };

    PlatformSprite::createPlatformGroup(platformSprites, platformPositions);

    std::vector<sf::Vector2f> springPositions = {
            sf::Vector2f(3488.0f, 883.0f),
            sf::Vector2f(3612.0f, 624.0f),
            sf::Vector2f(3952.0f, 951.0f)
    };

    SpringSprite::createSpringGroup(springSprites, springPositions);



    for (const auto& pos : buzzerPositions) {
        buzzerEnemies.push_back(new BuzzerEnemy(pos));}

    std::vector<sf::Vector2f> motobugPositions = {
            sf::Vector2f(208 * 4, 232 * 4),
            sf::Vector2f(1302 * 4, 190 * 4)
    };

    for (const auto& pos : motobugPositions) {
        auto* motobug = new MotobugEnemy(pos);
        motobug->setCollisionMap(collision);
        motobugEnemies.push_back(motobug);
    }

    std::vector<sf::Vector2f> crabmeatPositions = {
            sf::Vector2f(548 * 4, 208 * 4),
            sf::Vector2f(558 * 4, 200 * 4),
            sf::Vector2f(2142 * 4, 287 * 4)
    };

    for (const auto& pos : crabmeatPositions) {
        auto* crabmeat = new CrabmeatEnemy(pos);
        crabmeat->setCollisionMap(collision);
        crabmeatEnemies.push_back(crabmeat);
    }

    auto* crabmeat1 = new CrabmeatEnemy(sf::Vector2f(5370, 480));
    crabmeat1->setCollisionMap(collision);
    crabmeatEnemies.push_back(crabmeat1);


    platformSprites.push_back(new PlatformSprite(sf::Vector2f(2322*2, 223*2)));

    fishEnemies.push_back(new FishEnemy(sf::Vector2f(295 * 4, 190 * 4), 265));
    fishEnemies.push_back(new FishEnemy(sf::Vector2f(669 * 4, 164 * 4), 225));
    fishEnemies.push_back(new FishEnemy(sf::Vector2f(690 * 4, 164 * 4), 238));
    fishEnemies.push_back(new FishEnemy(sf::Vector2f(2018 * 4, 164 * 4), 240));
    fishEnemies.push_back(new FishEnemy(sf::Vector2f(2031 * 4, 164 * 4), 243));


    if (!gameFont.openFromFile("./assets/arial.ttf")) {
        throw std::runtime_error("fail");
    }

    initText();
}




void GameEngine::CreateScatteredRing(const sf::Vector2f& position, const sf::Vector2f& velocity) {
    try {
        scatteredRings.push_back(ScatteredRing(position, velocity));


    }
    catch (const std::exception& e) {
        std::cerr << "fail" << e.what() << std::endl;
    }
}


GameEngine::ScatteredRing::ScatteredRing(const sf::Vector2f& pos, const sf::Vector2f& vel)
        : velocity(vel)
        , lifetime(0.0f)
        , floorTime(0.0f)
        , active(true)
        , onFloor(false)
{
    sprite = std::make_unique<RingSprite>(pos);
}

void GameEngine::initPowerUpSprites() {
    powerUpSprites.push_back(new PowerUpSprite(sf::Vector2f(144.0f * 4, 208.0f * 4), PowerUpSprite::PowerUpType::SPEED));
    powerUpSprites.push_back(new PowerUpSprite(sf::Vector2f(369.0f * 4, 196.0f * 4), PowerUpSprite::PowerUpType::HEALTH));
    powerUpSprites.push_back(new PowerUpSprite(sf::Vector2f(1196.0f * 4, 200.0f * 4), PowerUpSprite::PowerUpType::SHIELD));
    powerUpSprites.push_back(new PowerUpSprite(sf::Vector2f(2188.0f * 4, 208.0f * 4), PowerUpSprite::PowerUpType::RINGS));
    powerUpSprites.push_back(new PowerUpSprite(sf::Vector2f(2249.0f * 4, 196.0f * 4), PowerUpSprite::PowerUpType::INVINCIBILITY));

    for (auto* powerUp : powerUpSprites) {
        powerUp->setEngineRef(this);
    }
}


void GameEngine::handlePowerUpEffect(PowerUpSprite::PowerUpType type) {
    if (!player) return;

    switch(type) {
        case PowerUpSprite::PowerUpType::RINGS:
            for (int i = 0; i < 10; i++) {
                player->addRing();
            }
            updateRingDisplay();
            break;

        case PowerUpSprite::PowerUpType::INVINCIBILITY:
            player->startInvincibility(20.0f);
            break;

        case PowerUpSprite::PowerUpType::SPEED:
            player->startSpeedBoost(10.0f);
            break;

        case PowerUpSprite::PowerUpType::SHIELD:
            player->addShield();
            break;

        case PowerUpSprite::PowerUpType::HEALTH:
            increasePlayerLives(1);
            updateLivesDisplay();
            break;
    }
}



void GameEngine::initFlowerSprites() {
    flowerSprites.push_back(new FlowerSprite(false, sf::Vector2f(0.0f, 880.0f)));
    flowerSprites.push_back(new FlowerSprite(false, sf::Vector2f(256.0f, 880.0f)));
    flowerSprites.push_back(new FlowerSprite(false, sf::Vector2f(512.0f, 880.0f)));
    flowerSprites.push_back(new FlowerSprite(false, sf::Vector2f(608.0f, 800.0f)));
    flowerSprites.push_back(new FlowerSprite(false, sf::Vector2f(768.0f, 880.0f)));
    flowerSprites.push_back(new FlowerSprite(false, sf::Vector2f(1408.0f, 832.0f)));
    flowerSprites.push_back(new FlowerSprite(false, sf::Vector2f(1792.0f, 880.0f)));
    flowerSprites.push_back(new FlowerSprite(false, sf::Vector2f(548.0f * 4, 200.0f * 4)));
    flowerSprites.push_back(new FlowerSprite(false, sf::Vector2f(768.0f * 4, 156.0f * 4)));
    flowerSprites.push_back(new FlowerSprite(false, sf::Vector2f(832.0f * 4, 156.0f * 4)));
    flowerSprites.push_back(new FlowerSprite(false, sf::Vector2f(1008.0f * 4, 146.0f * 4)));
    flowerSprites.push_back(new FlowerSprite(false, sf::Vector2f(1016.0f * 4, 140.0f * 4)));
    flowerSprites.push_back(new FlowerSprite(false, sf::Vector2f(1028.0f * 4, 216.0f * 4)));
    flowerSprites.push_back(new FlowerSprite(false, sf::Vector2f(1124.0f * 4, 136.0f * 4)));
    flowerSprites.push_back(new FlowerSprite(false, sf::Vector2f(1164.0f * 4, 130.0f * 4)));
    flowerSprites.push_back(new FlowerSprite(false, sf::Vector2f(1280.0f * 4, 156.0f * 4)));
    flowerSprites.push_back(new FlowerSprite(false, sf::Vector2f(1412.0f * 4, 152.0f * 4)));
    flowerSprites.push_back(new FlowerSprite(false, sf::Vector2f(1476.0f * 4, 88.0f * 4)));
    flowerSprites.push_back(new FlowerSprite(false, sf::Vector2f(1536.0f * 4, 12.0f * 4)));
    flowerSprites.push_back(new FlowerSprite(false, sf::Vector2f(1560.0f * 4, 16.0f * 4)));
    flowerSprites.push_back(new FlowerSprite(false, sf::Vector2f(1600.0f * 4, 12.0f * 4)));
    flowerSprites.push_back(new FlowerSprite(false, sf::Vector2f(1624.0f * 4, 16.0f * 4)));
    flowerSprites.push_back(new FlowerSprite(false, sf::Vector2f(1664.0f * 4, 12.0f * 4)));
    flowerSprites.push_back(new FlowerSprite(false, sf::Vector2f(1688.0f * 4, 16.0f * 4)));
    flowerSprites.push_back(new FlowerSprite(false, sf::Vector2f(1792.0f * 4, 284.0f * 4)));
    flowerSprites.push_back(new FlowerSprite(false, sf::Vector2f(1888.0f * 4, 208.0f * 4)));
    flowerSprites.push_back(new FlowerSprite(false, sf::Vector2f(2096.0f * 4, 208.0f * 4)));
    flowerSprites.push_back(new FlowerSprite(false, sf::Vector2f(2104.0f * 4, 204.0f * 4)));
    flowerSprites.push_back(new FlowerSprite(false, sf::Vector2f(2176.0f * 4, 220.0f * 4)));
    flowerSprites.push_back(new FlowerSprite(false, sf::Vector2f(2200.0f * 4, 200.0f * 4)));
    flowerSprites.push_back(new FlowerSprite(false, sf::Vector2f(2304.0f * 4, 284.0f * 4)));
    flowerSprites.push_back(new FlowerSprite(false, sf::Vector2f(2368.0f * 4, 284.0f * 4)));
    flowerSprites.push_back(new FlowerSprite(false, sf::Vector2f(2432.0f * 4, 284.0f * 4)));
    flowerSprites.push_back(new FlowerSprite(false, sf::Vector2f(2496.0f * 4, 284.0f * 4)));
    flowerSprites.push_back(new FlowerSprite(true, sf::Vector2f(64.0f, 904.0f)));
    flowerSprites.push_back(new FlowerSprite(true, sf::Vector2f(96.0f, 904.0f)));
    flowerSprites.push_back(new FlowerSprite(true, sf::Vector2f(320.0f, 904.0f)));
    flowerSprites.push_back(new FlowerSprite(true, sf::Vector2f(352.0f, 904.0f)));
    flowerSprites.push_back(new FlowerSprite(true, sf::Vector2f(1312.0f, 840.0f)));
    flowerSprites.push_back(new FlowerSprite(true, sf::Vector2f(1344.0f, 840.0f)));
    flowerSprites.push_back(new FlowerSprite(true, sf::Vector2f(1440.0f, 840.0f)));
    flowerSprites.push_back(new FlowerSprite(true, sf::Vector2f(2080.0f, 840.0f)));
    flowerSprites.push_back(new FlowerSprite(true, sf::Vector2f(784.0f * 4, 162.0f * 4)));
    flowerSprites.push_back(new FlowerSprite(true, sf::Vector2f(792.0f * 4, 162.0f * 4)));
    flowerSprites.push_back(new FlowerSprite(true, sf::Vector2f(912.0f * 4, 210.0f * 4)));
    flowerSprites.push_back(new FlowerSprite(true, sf::Vector2f(948.0f * 4, 226.0f * 4)));
    flowerSprites.push_back(new FlowerSprite(true, sf::Vector2f(1096.0f * 4, 146.0f * 4)));
    flowerSprites.push_back(new FlowerSprite(true, sf::Vector2f(1232.0f * 4, 146.0f * 4)));
    flowerSprites.push_back(new FlowerSprite(true, sf::Vector2f(1268.0f * 4, 162.0f * 4)));
    flowerSprites.push_back(new FlowerSprite(true, sf::Vector2f(1296.0f * 4, 162.0f * 4)));
    flowerSprites.push_back(new FlowerSprite(true, sf::Vector2f(1304.0f * 4, 162.0f * 4)));
    flowerSprites.push_back(new FlowerSprite(true, sf::Vector2f(1552.0f * 4, 18.0f * 4)));
    flowerSprites.push_back(new FlowerSprite(true, sf::Vector2f(1576.0f * 4, 18.0f * 4)));
    flowerSprites.push_back(new FlowerSprite(true, sf::Vector2f(1588.0f * 4, 18.0f * 4)));
    flowerSprites.push_back(new FlowerSprite(true, sf::Vector2f(1616.0f * 4, 18.0f * 4)));
    flowerSprites.push_back(new FlowerSprite(true, sf::Vector2f(1640.0f * 4, 18.0f * 4)));
    flowerSprites.push_back(new FlowerSprite(true, sf::Vector2f(1652.0f * 4, 18.0f * 4)));
    flowerSprites.push_back(new FlowerSprite(true, sf::Vector2f(1680.0f * 4, 18.0f * 4)));
    flowerSprites.push_back(new FlowerSprite(true, sf::Vector2f(1704.0f * 4, 18.0f * 4)));
    flowerSprites.push_back(new FlowerSprite(true, sf::Vector2f(1716.0f * 4, 18.0f * 4)));
    flowerSprites.push_back(new FlowerSprite(true, sf::Vector2f(1744.0f * 4, 274.0f * 4)));
    flowerSprites.push_back(new FlowerSprite(true, sf::Vector2f(1780.0f * 4, 290.0f * 4)));
    flowerSprites.push_back(new FlowerSprite(true, sf::Vector2f(1824.0f * 4, 210.0f * 4)));
    flowerSprites.push_back(new FlowerSprite(true, sf::Vector2f(1872.0f * 4, 210.0f * 4)));
    flowerSprites.push_back(new FlowerSprite(true, sf::Vector2f(1896.0f * 4, 210.0f * 4)));
    flowerSprites.push_back(new FlowerSprite(true, sf::Vector2f(2256.0f * 4, 274.0f * 4)));
    flowerSprites.push_back(new FlowerSprite(true, sf::Vector2f(2292.0f * 4, 290.0f * 4)));
    flowerSprites.push_back(new FlowerSprite(true, sf::Vector2f(2320.0f * 4, 290.0f * 4)));
    flowerSprites.push_back(new FlowerSprite(true, sf::Vector2f(2328.0f * 4, 290.0f * 4)));
    flowerSprites.push_back(new FlowerSprite(true, sf::Vector2f(2384.0f * 4, 290.0f * 4)));
    flowerSprites.push_back(new FlowerSprite(true, sf::Vector2f(2392.0f * 4, 290.0f * 4)));
    flowerSprites.push_back(new FlowerSprite(true, sf::Vector2f(2448.0f * 4, 290.0f * 4)));
    flowerSprites.push_back(new FlowerSprite(true, sf::Vector2f(2456.0f * 4, 290.0f * 4)));
    flowerSprites.push_back(new FlowerSprite(true, sf::Vector2f(2512.0f * 4, 290.0f * 4)));
    flowerSprites.push_back(new FlowerSprite(true, sf::Vector2f(2520.0f * 4, 290.0f * 4)));
}


void GameEngine::updateScatteredRings(float deltaTime) {
    const float RING_GRAVITY = 0.3f;
    const float RING_BOUNCE = -0.5f;
    const float FLOOR_LIFETIME = 6.0f;
    const float COLLECTION_DELAY = 0.5f;
    const float FLASH_START_TIME = 2.0f;

    sf::FloatRect playerBounds;
    bool canCollect = false;
    if (player) {
        playerBounds = player->getCollisionBounds();
        canCollect = true;
    }

    auto it = scatteredRings.begin();
    while (it != scatteredRings.end()) {
        if (!it->isActive() || !it->getSprite()) {
            it = scatteredRings.erase(it);
            continue;
        }

        it->updateLifetime(deltaTime);

        if (canCollect && it->getLifetime() > COLLECTION_DELAY && it->isActive()) {
            if (playerBounds.findIntersection(it->getSprite()->getBounds())) {
                player->addRing();
                updateRingDisplay();
                it = scatteredRings.erase(it);
                continue;
            }
        }



        bool isOnFloor = it->isOnFloor();

        if (isOnFloor) {
            it->updateFloorTime(deltaTime);
            float remainingTime = FLOOR_LIFETIME - it->getFloorTime();

            if (remainingTime <= 0) {
                it = scatteredRings.erase(it);
                continue;
            }

            if (remainingTime < FLASH_START_TIME) {
                float flashFreq = 10.0f + (20.0f * (1.0f - (remainingTime / FLASH_START_TIME)));
                float alpha = (std::sin(it->getFloorTime() * flashFreq) + 1.0f) * 0.5f;
                it->getSprite()->setOpacity(static_cast<uint8_t>(alpha * 255));
            }
        } else {
            sf::Vector2f vel = it->getVelocity();
            vel.y += RING_GRAVITY;
            it->setVelocity(vel);

            sf::Vector2f newPos = it->getSprite()->getPosition() + vel;
            sf::FloatRect ringBounds(newPos, sf::Vector2f(16.0f, 16.0f));

            if (collision->checkCollision(ringBounds, vel.y)) {
                if (std::abs(vel.y) < 1.0f) {
                    it->setVelocity(sf::Vector2f(0.0f, 0.0f));
                    it->setOnFloor(true);
                } else {
                    vel.y = RING_BOUNCE * std::abs(vel.y);
                    it->setVelocity(vel);
                }
                newPos.y = it->getSprite()->getPosition().y;
            }

            it->getSprite()->setPosition(newPos);
        }

        it->getSprite()->update(deltaTime);
        ++it;
    }
}



void GameEngine::renderScatteredRings() {
    if (!window) return;

    for (const auto& ring : scatteredRings) {
        if (ring.isActive() && ring.getSprite()) {
            ring.getSprite()->render(*window);
        }
    }
}



void GameEngine::updateRingDisplay() {
    if (player && ringCountText) {
        std::string ringStr = "RINGS: " + std::to_string(player->getRingCount());
        ringCountText->setString(ringStr);

        auto windowSize = window->getSize();
        ringCountText->setPosition(sf::Vector2f(20.f, 20.f));

        ringCountText->setCharacterSize(24);
        ringCountText->setFillColor(sf::Color(255, 215, 0));
        ringCountText->setOutlineColor(sf::Color::Black);
        ringCountText->setOutlineThickness(1.0f);
    }
}

void GameEngine::initText() {
    if (ringCountText == nullptr) {
        ringCountText = new sf::Text(gameFont, "RINGS: 0", 24);
        ringCountText->setFillColor(sf::Color(255, 215, 0));
        ringCountText->setOutlineColor(sf::Color::Black);
        ringCountText->setOutlineThickness(1.0f);
        ringCountText->setPosition(sf::Vector2f(20.f, 20.f));
    }

    if (livesCountText == nullptr) {
        livesCountText = new sf::Text(gameFont, "LIVES: " + std::to_string(currentLives), 24);
        livesCountText->setFillColor(sf::Color(255, 100, 100));
        livesCountText->setOutlineColor(sf::Color::Black);
        livesCountText->setOutlineThickness(1.0f);
        livesCountText->setPosition(sf::Vector2f(20.f, 50.f));
    }
}



void GameEngine::updateLivesDisplay() {
    if (livesCountText) {
        livesCountText->setString("LIVES: " + std::to_string(currentLives));
    }
}

void GameEngine::handlePlayerDeath() {
    if (currentLives > 0) {
        currentLives--;
        updateLivesDisplay();
        respawnPlayer();
    } else {
        SetCurrentState(GameState::GAME_OVER);
    }
}

void GameEngine::respawnPlayer() {
    if (!player || !map) return;

    sf::Vector2f spawnPos = getSpawnPosition();

    player->setPosition(spawnPos);

    resetRings();
    resetEnemies();

    player->resetAfterDeath();

    view.setCenter(spawnPos);
    constrainView();
}





void GameEngine::constrainView() {
    float viewHalfHeight = view.getSize().y / 2.0f;
    float viewHalfWidth = view.getSize().x / 2.0f;
    auto center = view.getCenter();

    center.x = std::clamp(center.x, minViewBounds.x, viewBounds.x);

    float maxY;
    if (center.x <= CAMERA_X_THRESHOLD) {
        maxY = CAMERA_Y_MAX_BEFORE_THRESHOLD - viewHalfHeight;
    } else {
        maxY = CAMERA_Y_MAX_AFTER_THRESHOLD - viewHalfHeight;
    }

    center.y = std::clamp(center.y, minViewBounds.y, maxY);

    view.setCenter(center);
}



void GameEngine::constrainBackgroundView() {
    auto center = bgr_view.getCenter();
    center.x = std::clamp(center.x, minBgrViewBounds.x, bgrViewBounds.x);
    center.y = std::clamp(center.y, minBgrViewBounds.y, bgrViewBounds.y);
    bgr_view.setCenter(center);
}






// Handles all input events (keyboard, mouse, window)
void GameEngine::poll() {

    while (const std::optional event = window->pollEvent()) {

        if (event->is<sf::Event::Closed>()) {

            window->close();

            return;

        }



        if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {

            if (keyPressed->scancode == sf::Keyboard::Scancode::Escape &&

                currentState == GameState::PLAYING) {

                isPaused = !isPaused;

            }

            else if (keyPressed->scancode == sf::Keyboard::Scancode::Enter &&

                     currentState == GameState::INTRO) {

                stateManager->handleIntroInput();

            }

        }

        else if (const auto* mouseClick = event->getIf<sf::Event::MouseButtonPressed>()) {

            if (mouseClick->button == sf::Mouse::Button::Left && isPaused) {

                stateManager->handlePauseMenuClick(mouseClick->position.x, mouseClick->position.y);

            }

        }

        else if (const auto* keyReleased = event->getIf<sf::Event::KeyReleased>()) {

            if (!isPaused && currentState == GameState::PLAYING) {

                if (keyReleased->scancode == sf::Keyboard::Scancode::W ||

                    keyReleased->scancode == sf::Keyboard::Scancode::S ||

                    keyReleased->scancode == sf::Keyboard::Scancode::A ||

                    keyReleased->scancode == sf::Keyboard::Scancode::D) {

                    player->resetAnimationTimer();

                }

            }

        }

    }

}


// Main update function that handles game state and logic updates
void GameEngine::update() {
    try {
        poll();

        static GameState lastState = currentState;
        if (lastState != currentState) {
            lastState = currentState;
        }

        switch (currentState) {
            case GameState::INTRO:
                if (stateManager) {
                    stateManager->updateIntroScreen();
                    if (stateManager->isIntroFadeComplete()) {
                        currentState = GameState::PLAYING;
                        if (!isMusicMuted && bgMusic.getStatus() != sf::Music::Status::Playing) {
                            bgMusic.play();
                        }
                    }
                }
                break;

            case GameState::PLAYING:
                if (!isPaused) {
                    updateGameState();
                }
                break;

            case GameState::PAUSED:
                break;

            case GameState::COMPLETED:
                if (stateManager) {
                    stateManager->handleCompletionInput();
                    stateManager->updateCompletionScreen();
                }
                break;

            case GameState::GAME_OVER:
                if (stateManager) {
                    stateManager->handleGameOverInput();
                }
                break;
        }
    }
    catch (const std::exception& e) {
        std::cerr << "error " << e.what() << std::endl;
        throw;
    }
}





// Updates the main game state including player, enemies, and collectibles

void GameEngine::updateGameState() {
    static sf::Clock clock;
    const float deltaTime = clock.restart().asSeconds();

    player->update();
    updateScatteredRings(deltaTime);

    if (player && player->getPosition().x >= LEVEL_END_X) {
        SetCurrentState(GameState::COMPLETED);
        return;
    }

    sf::Vector2f playerCenter = player->getPosition();
    sf::Vector2f currentCenter = view.getCenter();
    float followSpeed = isGodMode ? 10.0f * deltaTime : 5.0f * deltaTime;
    sf::Vector2f newCenter = currentCenter + (playerCenter - currentCenter) * followSpeed;

    if (!isGodMode && player && !player->IsDead()) {
        const auto& spikes = GetSpikeSprites();
        player->checkSpikeCollisions(spikes);
    }

    if (isGodMode) {
        view.setCenter(playerCenter);
    } else {
        view.setCenter(newCenter);
        constrainView();
    }

    sf::Vector2f viewMovement = newCenter - currentCenter;
    bgr_view.move(sf::Vector2f{viewMovement.x * PARALLAX_FACTOR, 0.f});
    if (!isGodMode) constrainBackgroundView();

    for (auto* flower : flowerSprites) {
        if (flower) flower->update(deltaTime);
    }

    for (auto* fish : fishEnemies) {
        if (fish) fish->update(deltaTime);
    }

    for (auto* crabmeat : crabmeatEnemies) {
        if (crabmeat) crabmeat->update(deltaTime);
    }

    for (auto* motobug : motobugEnemies) {
        if (motobug) motobug->update(deltaTime);
    }

    for (auto* buzzer : buzzerEnemies) {
        if (buzzer) buzzer->update(deltaTime);
    }

    for (auto* powerUp : powerUpSprites) {
        if (powerUp) powerUp->update(deltaTime);
    }

    sf::FloatRect playerBounds = player->getCollisionBounds();

    auto fishIt = fishEnemies.begin();
    while (fishIt != fishEnemies.end()) {
        FishEnemy* fish = *fishIt;
        if (!fish) {
            fishIt = fishEnemies.erase(fishIt);
            continue;
        }

        fish->update(deltaTime);

        if (fish->isAlive() && playerBounds.findIntersection(fish->getCollisionBounds())) {
            if (player->isInBallState()) {
                fish->die();
                if (!fish->getFreedAnimal()) {
                    delete fish;
                    fishIt = fishEnemies.erase(fishIt);
                } else {
                    ++fishIt;
                }
                continue;
            } else {
                player->handleDamage();
            }
        }
        ++fishIt;
    }

    auto crabmeatIt = crabmeatEnemies.begin();
    while (crabmeatIt != crabmeatEnemies.end()) {
        CrabmeatEnemy* crabmeat = *crabmeatIt;
        if (!crabmeat) {
            crabmeatIt = crabmeatEnemies.erase(crabmeatIt);
            continue;
        }

        crabmeat->update(deltaTime);

        if (crabmeat->isAlive() && playerBounds.findIntersection(crabmeat->getCollisionBounds())) {
            if (player->isInBallState()) {
                crabmeat->die();
                ++crabmeatIt;
                continue;
            } else {
                player->handleDamage();
            }
        }

        if (crabmeat->isAlive() && !player->IsDead() && !player->IsHurt()) {
            if (crabmeat->checkPlayerInRange(player->getCollisionBounds())) {
                crabmeat->shoot();
            }
            if (!player->IsInvincible() &&
                crabmeat->checkProjectileCollision(player->getCollisionBounds())) {
                player->handleDamage();
            }
        }

        if (!crabmeat->isAlive() && (!crabmeat->getFreedAnimal() || crabmeat->getFreedAnimal()->shouldBeDeleted())) {
            delete crabmeat;
            crabmeatIt = crabmeatEnemies.erase(crabmeatIt);
        } else {
            ++crabmeatIt;
        }
    }

    auto motobugIt = motobugEnemies.begin();
    while (motobugIt != motobugEnemies.end()) {
        MotobugEnemy* motobug = *motobugIt;
        if (!motobug) {
            motobugIt = motobugEnemies.erase(motobugIt);
            continue;
        }

        motobug->update(deltaTime);

        if (motobug->isAlive() && playerBounds.findIntersection(motobug->getCollisionBounds())) {
            if (player->isInBallState()) {
                motobug->die();
                if (!motobug->getFreedAnimal()) {
                    delete motobug;
                    motobugIt = motobugEnemies.erase(motobugIt);
                } else {
                    ++motobugIt;
                }
                continue;
            } else {
                player->handleDamage();
            }
        }
        ++motobugIt;
    }

    auto buzzerIt = buzzerEnemies.begin();
    while (buzzerIt != buzzerEnemies.end()) {
        BuzzerEnemy* buzzer = *buzzerIt;
        if (!buzzer) {
            buzzerIt = buzzerEnemies.erase(buzzerIt);
            continue;
        }

        buzzer->update(deltaTime);

        if (buzzer->isAlive()) {
            if (player && buzzer->checkPlayerInRange(player->getCollisionBounds())) {
                buzzer->shoot(player->getPosition());
            }

            if (playerBounds.findIntersection(buzzer->getCollisionBounds())) {
                if (player->isInBallState()) {
                    buzzer->die();
                    if (!buzzer->getFreedAnimal()) {
                        delete buzzer;
                        buzzerIt = buzzerEnemies.erase(buzzerIt);
                    } else {
                        ++buzzerIt;
                    }
                    continue;
                } else {
                    player->handleDamage();
                }
            }

            if (player && !player->IsInvincible() &&
                buzzer->checkProjectileCollision(player->getCollisionBounds())) {
                player->handleDamage();
            }
        }
        ++buzzerIt;
    }

    for (auto* checkpoint : checkpointSprites) {
        if (checkpoint && !checkpoint->isActive() &&
            playerBounds.findIntersection(checkpoint->getCollisionBounds())) {
            checkpoint->activate();
            handleCheckpointActivation(checkpoint->getPosition());
        }
        checkpoint->update(deltaTime);
    }

    for (auto* spring : springSprites) {
        if (!spring || !player) continue;

        if (!player->IsDead() && !player->IsHurt()) {
            sf::FloatRect playerBounds = player->getCollisionBounds();
            sf::FloatRect springBounds = spring->getCollisionBounds();

            bool isAboveSpring = playerBounds.position.y + playerBounds.size.y <= springBounds.position.y + 5.0f;
            bool isMovingDown = player->velocity.y > 0;
            bool canBounce = spring->canBounce();

            if (playerBounds.findIntersection(springBounds) &&
                isAboveSpring &&
                isMovingDown &&
                canBounce) {

                spring->extend();
                player->bounceFromSpring();
            }
        }

        spring->update(deltaTime);
    }


    auto ringIt = ringSprites.begin();
    while (ringIt != ringSprites.end()) {
        RingSprite* ring = *ringIt;
        if (!ring) {
            ringIt = ringSprites.erase(ringIt);
            continue;
        }

        if (!ring->isActive()) {
            delete ring;
            ringIt = ringSprites.erase(ringIt);
            continue;
        }

        if (playerBounds.findIntersection(ring->getBounds())) {
            if (!ring->isCollected()) {
                ring->collect();
                player->addRing();
                updateRingDisplay();
            }
        }

        ring->update(deltaTime);
        ++ringIt;
    }
}


void GameEngine::setGodMode(bool enabled) {

    isGodMode = enabled;

    if (player) {

        player->setGodMode(enabled);

    }

}



// Stores the initial positions of all enemies for respawn purposes

void GameEngine::storeInitialEnemyPositions() {
    for (auto* buzzer : buzzerEnemies) {
        buzzerInitialStates[buzzer] = {buzzer->getPosition(), true};
    }
    for (auto* motobug : motobugEnemies) {
        motobugInitialStates[motobug] = {motobug->getPosition(), true};
    }
    for (auto* crabmeat : crabmeatEnemies) {
        crabmeatInitialStates[crabmeat] = {crabmeat->getPosition(), true};
    }
    for (auto* fish : fishEnemies) {
        fishInitialStates[fish] = {fish->getPosition(), true};
    }
}


void GameEngine::resetRings() {
    scatteredRings.clear();

    for (auto* ring : ringSprites) {
        if (ring) {
            ring->uncollect();
        }
    }

    ringSprites.erase(
            std::remove_if(ringSprites.begin(), ringSprites.end(),
                           [](RingSprite* ring) { return ring == nullptr; }),
            ringSprites.end()
    );

    if (player) {
        player->loseRings();
        updateRingDisplay();
    }
}


void GameEngine::resetEnemies() {
    for (auto* buzzer : buzzerEnemies) {
        if (buzzer) {
            buzzer->setPosition(buzzerInitialStates[buzzer].position);
            buzzer->reset();
        }
    }

    for (auto* motobug : motobugEnemies) {
        if (motobug) {
            motobug->setPosition(motobugInitialStates[motobug].position);
            motobug->reset();
        }
    }

    for (auto* crabmeat : crabmeatEnemies) {
        if (crabmeat) {
            crabmeat->setPosition(crabmeatInitialStates[crabmeat].position);
            crabmeat->reset();
        }
    }

    for (auto* fish : fishEnemies) {
        if (fish) {
            fish->setPosition(fishInitialStates[fish].position);
            fish->reset();
        }
    }
}



void GameEngine::render() {
    try {
        if (!window) {
            throw std::runtime_error("Window is null");
        }

        window->clear();

        switch (currentState) {
            case GameState::INTRO:
                if (stateManager) {
                    window->setView(window->getDefaultView());
                    stateManager->renderIntroScreen();
                }
                break;

            case GameState::PLAYING:
                renderPlayingState();
                window->setView(window->getDefaultView());
                if (ringCountText) {
                    window->draw(*ringCountText);
                }
                if (livesCountText) {
                    window->draw(*livesCountText);
                }
                break;

            case GameState::PAUSED:
                renderPausedState();
                break;

            case GameState::COMPLETED:
                renderPlayingState();
                window->setView(window->getDefaultView());
                if (stateManager) {
                    stateManager->renderCompletionScreen();
                }
                break;

            case GameState::GAME_OVER:
                renderPlayingState();
                window->setView(window->getDefaultView());
                if (stateManager) {
                    stateManager->renderGameOverScreen();
                }
                break;
        }

        window->display();
    }
    catch (const std::exception& e) {
        std::cerr << "error: " << e.what() << std::endl;
        throw;
    }
}


void GameEngine::renderPlayingState() {
    if (!window || !bgr || !map || !collision || !player) {
        throw std::runtime_error("null");
    }

    window->setView(bgr_view);
    bgr->render(*window, BG_SCALE);

    window->setView(view);
    map->render(*window, 1.0f);




    if (isGridMapVisible) {

        collision->renderCollisionDebug(*window, 1.0f);

    }



    for (auto* flower : flowerSprites) {

        if (flower) {

            flower->render(*window);

        }

    }

    for (auto* bridge : bridgeSprites) {
        if (bridge) {
            bridge->render(*window);
        }
    }

    for (auto* spring : springSprites) {
        if (spring) {
            spring->render(*window);
        }
    }

    for (auto* motobug : motobugEnemies) {
        if (motobug) {
            motobug->render(*window);
        }
    }

    for (auto* crabmeat : crabmeatEnemies) {
        if (crabmeat) {
            crabmeat->render(*window);
        }
    }

    for (auto* fish : fishEnemies) {
        if (fish) {
            fish->setCollisionMap(collision);
        }
    }

    for (auto* fish : fishEnemies) {
        if (fish) {
            fish->render(*window);
        }
    }

    for (auto* buzzer : buzzerEnemies) {
        if (buzzer) {
            buzzer->render(*window);
        }
    }

    for (auto* buzzer : buzzerEnemies) {
        if (buzzer) {
            buzzer->setCollisionMap(collision);
        }
    }

    for (auto* powerUp : powerUpSprites) {
        if (powerUp && player) {
            sf::FloatRect bounds = player->getCollisionBounds();

            if (powerUp->checkCollision(bounds)) {
                if (player->isInBallState()) {
                    powerUp->breakBox();
                } else {
                }
            }

            powerUp->render(*window);
        }
    }


    for (auto* spike : spikeSprites) {
        if (spike) {
            spike->render(*window);
        }
    }
    for (auto* checkpoint : checkpointSprites) {
        if (checkpoint) {
            checkpoint->render(*window);
        }
    }

    for (const auto& ring : scatteredRings) {
        if (ring.active && ring.sprite) {
            ring.sprite->render(*window);
        }
    }

    for (auto* ring : ringSprites) {
        if (ring) {
            ring->render(*window);
        }
    }

    for (auto* platform : platformSprites) {
        if (platform) {
            platform->render(*window);
        }
    }

    renderScatteredRings();
    player->render(*window);



    if (isPaused) {

        window->setView(window->getDefaultView());

        if (stateManager) {

            stateManager->renderPauseMenu();

        }

    }

}





void GameEngine::renderPausedState() {



    window->setView(bgr_view);

    if (bgr) bgr->render(*window, BG_SCALE);



    window->setView(view);

    if (map) map->render(*window, 1.0f);

    if (collision) collision->renderCollisionDebug(*window, 1.0f);



    for (auto* flower : flowerSprites) {
        if (flower) flower->render(*window);
    }


    if (player) player->render(*window);


    window->setView(window->getDefaultView());

    if (stateManager) {
        stateManager->renderPauseMenu();
    }

}


void GameEngine::handleCheckpointActivation(const sf::Vector2f& checkpointPos) {
    lastCheckpoint = checkpointPos;

}

sf::Vector2f GameEngine::getSpawnPosition() const {
    if (lastCheckpoint) {
        return *lastCheckpoint;
    }

    return sf::Vector2f(
            view.getSize().x / 2.0f,
            map->getMapHeight() * 256.0f - view.getSize().y * 3.0f
    );
}

void GameEngine::increasePlayerLives(int amount) {
    currentLives += amount;
    updateLivesDisplay();
}




void GameEngine::resetGame() {
    currentState = GameState::INTRO;
    isPaused = false;
    pauseTime = 0;

    currentLives = INITIAL_LIVES;
    updateLivesDisplay();

    lastCheckpoint.reset();
    for (auto* checkpoint : checkpointSprites) {
        if (checkpoint) {
            checkpoint->reset();
        }
    }
    resetEnemies();
    resetRings();


    if (player) {
        sf::Vector2f spawnPos = sf::Vector2f(
                view.getSize().x / 2.0f,
                map->getMapHeight() * 256.0f - view.getSize().y * 3.0f
        );

        player->setPosition(spawnPos);
        player->resetGame();
    }

    view.setCenter(sf::Vector2f(
            view.getSize().x / 2.0f,
            map->getMapHeight() * 256.0f - view.getSize().y / 2.0f
    ));

    bgr_view.setCenter(sf::Vector2f(
            BG_SCALE * view.getCenter().x,
            BG_SCALE * view.getSize().y / 2.0f
    ));

    if (!isMusicMuted) {
        bgMusic.stop();
        bgMusic.play();
    }


}


// Checks if the game window is still running

bool GameEngine::running() const { return window->isOpen(); }
