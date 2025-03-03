#include "../include/GameStateManager.h"
#include "../include/GameEngine.h"
#include "SoundManager.h"
#include <stdexcept>
#include <iostream>
#include <algorithm>
#include <filesystem>

GameStateManager::GameStateManager(sf::RenderWindow* window, sf::Music& music, float& volume, bool& muted, bool& godMode, bool& gridVisible)
        : window(window)
        , bgMusic(music)
        , musicVolume(volume)
        , isMusicMuted(muted)
        , isGodMode(godMode)
        , isGridMapVisible(gridVisible)
{
    try {
        initIntroScreen();
        initPauseMenu();
        initCompletionScreen();
        initGameOverScreen();
        bgMusic.stop();


        SoundManager::getInstance().setVolume(musicVolume);
    }
    catch (const std::exception& e) {
        cleanup();
        throw;
    }
}

GameStateManager::~GameStateManager() {
    cleanup();
}


void GameStateManager::cleanup() {
    delete introSprite;
    delete introText;
    delete pressEnterText;
    delete pauseText;
    delete volumeText;
    delete musicText;
    delete godModeText;
    delete completionText;
    delete gridMapText;
    delete gameOverText;
    gameOverText = nullptr;
    introSprite = nullptr;
    introText = nullptr;
    pressEnterText = nullptr;
    pauseText = nullptr;
    volumeText = nullptr;
    musicText = nullptr;
    godModeText = nullptr;
    completionText = nullptr;
    gridMapText = nullptr;
}


void GameStateManager::initIntroScreen() {

    if (!introTexture.loadFromFile("./assets/intro.png")) {
        throw std::runtime_error("fail");
    }


    introSprite = new sf::Sprite(introTexture);
    float scaleX = static_cast<float>(window->getSize().x) / introTexture.getSize().x;
    float scaleY = static_cast<float>(window->getSize().y) / introTexture.getSize().y;
    introSprite->setScale(sf::Vector2f(scaleX, scaleY));


    if (!introFont.openFromFile("./assets/arial.ttf")) {
        throw std::runtime_error("fail");
    }


    introText = new sf::Text(introFont,
                             "University of Crete\n"
                             "Department of Computer Science\n"
                             "CS-454. Development of Intelligent Interfaces and Games\n"
                             "Term Project, Fall Semester 2024\n\n"
                             "Team Members:\n"
                             "CSD4897 CSD5014 CSD5176",
                             24);
    introText->setFillColor(sf::Color::White);


    pressEnterText = new sf::Text(introFont, "Press Enter to Start", 30);
    pressEnterText->setFillColor(sf::Color::White);


    centerText(introText, -100.f);
    centerText(pressEnterText, 100.f);


    fadeAlpha = 255.0f;
    fadingOut = false;
    completionFadeAlpha = 0.0f;
    completionFadingIn = true;
}


void GameStateManager::initPauseMenu() {
    if (!window) {
        throw std::runtime_error("null window");
    }

    auto windowSize = window->getSize();

    pauseText = new sf::Text(introFont, "PAUSED", 50);
    volumeText = new sf::Text(introFont, "Volume: " + std::to_string(static_cast<int>(musicVolume)) + "%", 30);
    musicText = new sf::Text(introFont, "Music", 30);
    godModeText = new sf::Text(introFont, "God Mode", 30);
    gridMapText = new sf::Text(introFont, "Grid Map", 30);

    volumeSliderBg.setSize(sf::Vector2f(200.f, 10.f));
    volumeSlider.setSize(sf::Vector2f(200.f * (musicVolume / 100.f), 10.f));
    musicToggle.setSize(sf::Vector2f(30.f, 30.f));
    godModeToggle.setSize(sf::Vector2f(30.f, 30.f));
    gridMapToggle.setSize(sf::Vector2f(30.f, 30.f));

    volumeSliderBg.setFillColor(sf::Color(100, 100, 100));
    volumeSlider.setFillColor(sf::Color::White);
    musicToggle.setFillColor(isMusicMuted ? sf::Color::Red : sf::Color::Green);
    godModeToggle.setFillColor(isGodMode ? sf::Color::Green : sf::Color::Red);
    gridMapToggle.setFillColor(isGridMapVisible ? sf::Color::Green : sf::Color::Red);

    centerText(pauseText, -100.f);
    centerText(volumeText, 0.f);
    centerText(musicText, 100.f);
    centerText(godModeText, 150.f);
    centerText(gridMapText, 200.f);

    volumeSliderBg.setPosition(sf::Vector2f(windowSize.x / 2.f - 100.f, windowSize.y / 2.f));
    volumeSlider.setPosition(volumeSliderBg.getPosition());
    musicToggle.setPosition(sf::Vector2f(windowSize.x / 2.f - 100.f, windowSize.y / 2.f + 100.f));
    godModeToggle.setPosition(sf::Vector2f(windowSize.x / 2.f - 100.f, windowSize.y / 2.f + 150.f));
    gridMapToggle.setPosition(sf::Vector2f(windowSize.x / 2.f - 100.f, windowSize.y / 2.f + 200.f));
}

void GameStateManager::initCompletionScreen() {
    completionText = new sf::Text(
            introFont,
            "Congratulations!\nLevel 1 Finished\n\nPress Enter to Restart\nPress Esc to Exit",
            30
    );

    if (!completionText) {
        throw std::runtime_error("Failed to create completion text");
    }

    completionText->setFillColor(sf::Color::White);


    auto bounds = completionText->getLocalBounds();
    auto windowSize = window->getSize();
    completionText->setOrigin(sf::Vector2f(
            bounds.size.x / 2.f,
            bounds.size.y / 2.f
    ));
    completionText->setPosition(sf::Vector2f(
            windowSize.x / 2.f,
            windowSize.y / 2.f
    ));
}

void GameStateManager::initGameOverScreen() {
    gameOverText = new sf::Text(
            introFont,
            "GAME OVER\n\nPress Enter to Restart\nPress Esc to Exit",
            30
    );

    if (!gameOverText) {
        throw std::runtime_error("Failed to create game over text");
    }

    gameOverText->setFillColor(sf::Color::White);


    auto bounds = gameOverText->getLocalBounds();
    auto windowSize = window->getSize();
    gameOverText->setOrigin(sf::Vector2f(
            bounds.size.x / 2.f,
            bounds.size.y / 2.f
    ));
    gameOverText->setPosition(sf::Vector2f(
            windowSize.x / 2.f,
            windowSize.y / 2.f
    ));
}

void GameStateManager::renderGameOverScreen() const {
    if (!window || !gameOverText) {
        return;
    }


    sf::RectangleShape overlay(sf::Vector2f(window->getSize()));
    sf::Color overlayColor = sf::Color::Black;
    overlayColor.a = 192;
    overlay.setFillColor(overlayColor);

    window->draw(overlay);
    window->draw(*gameOverText);
}

void GameStateManager::handleGameOverInput() {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Enter)) {
        if (engineRef) {
            engineRef->resetGame();
        }
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape)) {
        if (window) {
            window->close();
        }
    }
}




void GameStateManager::centerText(sf::Text* text, float yOffset) {
    if (!text || !window) return;
    auto bounds = text->getLocalBounds();
    auto windowSize = window->getSize();
    text->setOrigin(sf::Vector2f(bounds.size.x / 2.f, bounds.size.y / 2.f));
    text->setPosition(sf::Vector2f(windowSize.x / 2.f, windowSize.y / 2.f + yOffset));
}

void GameStateManager::renderIntroScreen() const {
    if (!window || !introSprite || !introText || !pressEnterText) return;

    sf::Color color = sf::Color::White;
    color.a = static_cast<uint8_t>(fadeAlpha);

    introSprite->setColor(color);
    introText->setFillColor(color);
    pressEnterText->setFillColor(color);

    window->draw(*introSprite);
    window->draw(*introText);
    window->draw(*pressEnterText);
}

void GameStateManager::updateIntroScreen() {
    if (fadingOut) {
        fadeAlpha = std::max(0.f, fadeAlpha - 3.0f);
        sf::Color color = sf::Color::White;
        color.a = static_cast<uint8_t>(fadeAlpha);

        if (introSprite) introSprite->setColor(color);
        if (introText) introText->setFillColor(color);
        if (pressEnterText) pressEnterText->setFillColor(color);
    }
}

void GameStateManager::handleIntroInput() {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Enter)) {
        fadingOut = true;
    }
}

void GameStateManager::renderPauseMenu() const {
    if (!window) return;

    sf::RectangleShape overlay(sf::Vector2f(window->getSize()));
    overlay.setFillColor(sf::Color(0, 0, 0, 128));

    window->draw(overlay);
    if (pauseText) window->draw(*pauseText);
    if (volumeText) window->draw(*volumeText);
    window->draw(volumeSliderBg);
    window->draw(volumeSlider);
    window->draw(musicToggle);
    if (musicText) window->draw(*musicText);
    window->draw(godModeToggle);
    if (godModeText) window->draw(*godModeText);
    if (gridMapText) window->draw(*gridMapText);
    window->draw(gridMapToggle);
}

void GameStateManager::handlePauseMenuClick(int x, int y) {
    sf::Vector2f mousePos(static_cast<float>(x), static_cast<float>(y));

    if (volumeSliderBg.getGlobalBounds().contains(mousePos)) {
        setMusicVolume((mousePos.x - volumeSliderBg.getPosition().x) / volumeSliderBg.getSize().x * 100.f);
    } else if (musicToggle.getGlobalBounds().contains(mousePos)) {
        toggleMusic();
    } else if (godModeToggle.getGlobalBounds().contains(mousePos)) {
        toggleGodMode();
    }

    if (gridMapToggle.getGlobalBounds().contains(mousePos)) {
        toggleGridMap();
    }
}

void GameStateManager::toggleGridMap() {
    isGridMapVisible = !isGridMapVisible;
    gridMapToggle.setFillColor(isGridMapVisible ? sf::Color::Green : sf::Color::Red);
}

void GameStateManager::toggleMusic() {
    isMusicMuted = !isMusicMuted;
    isMusicMuted ? bgMusic.pause() : bgMusic.play();
    musicToggle.setFillColor(isMusicMuted ? sf::Color::Red : sf::Color::Green);
}

void GameStateManager::toggleGodMode() {
    isGodMode = !isGodMode;
    godModeToggle.setFillColor(isGodMode ? sf::Color::Green : sf::Color::Red);
    if (engineRef) {
        engineRef->setGodMode(isGodMode);
    }
}

void GameStateManager::setMusicVolume(float volume) {
    musicVolume = std::clamp(volume, 0.0f, 100.0f);

    bgMusic.setVolume(musicVolume);

    SoundManager::getInstance().setVolume(musicVolume);


    volumeText->setString("Volume: " + std::to_string(static_cast<int>(musicVolume)) + "%");
    volumeSlider.setSize(sf::Vector2f(volumeSliderBg.getSize().x * (musicVolume / 100.f), 10.f));
}

void GameStateManager::updateCompletionScreen() {
    static sf::Clock fadeTimer;

    if (completionFadingIn) {
        float fadeSpeed = 2.0f;
        completionFadeAlpha = std::min(255.0f, completionFadeAlpha + (fadeSpeed * fadeTimer.restart().asSeconds() * 60.0f));

        sf::Color textColor = sf::Color::White;
        textColor.a = static_cast<uint8_t>(completionFadeAlpha);
        if (completionText) {
            completionText->setFillColor(textColor);
        }
    }
}

void GameStateManager::renderCompletionScreen() const {
    if (!window || !completionText) {
        return;
    }

    sf::RectangleShape overlay(sf::Vector2f(window->getSize()));
    sf::Color overlayColor = sf::Color::Black;
    overlayColor.a = static_cast<uint8_t>(completionFadeAlpha * 0.75f);
    overlay.setFillColor(overlayColor);

    window->draw(overlay);
    window->draw(*completionText);
}

void GameStateManager::handleCompletionInput() {
    if (completionFadeAlpha >= 255.0f) {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Enter)) {
            if (engineRef) {
                engineRef->resetGame();
            }
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape)) {
            if (window) {
                window->close();
            }
        }
    }
}