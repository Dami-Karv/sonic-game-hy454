#ifndef GAMESTATEMANAGER_H
#define GAMESTATEMANAGER_H

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "GameState.h"


class GameEngine;

class GameStateManager final {
private:

    sf::RenderWindow* window;
    sf::Text* gameOverText = nullptr;


    sf::Texture introTexture;
    sf::Sprite* introSprite;
    sf::Font introFont;
    sf::Text* introText;
    sf::Text* pressEnterText;


    sf::Text* pauseText;
    sf::Text* volumeText;
    sf::Text* musicText;
    sf::Text* godModeText;
    sf::Text* completionText;
    sf::Text* gridMapText;


    float fadeAlpha;
    bool fadingOut;
    float completionFadeAlpha;
    bool completionFadingIn;


    sf::RectangleShape volumeSliderBg;
    sf::RectangleShape volumeSlider;
    sf::RectangleShape musicToggle;
    sf::RectangleShape godModeToggle;
    sf::RectangleShape gridMapToggle;


    sf::Music& bgMusic;
    float& musicVolume;
    bool& isMusicMuted;
    bool& isGodMode;
    bool& isGridMapVisible;
    GameEngine* engineRef;


    void initIntroScreen();
    void initPauseMenu();
    void initCompletionScreen();
    void centerText(sf::Text* text, float yOffset = 0.0f);


    void cleanup();

public:

    void resetCompletionScreen() {
        completionFadeAlpha = 0.0f;
        completionFadingIn = true;
    }

    GameStateManager(sf::RenderWindow* window, sf::Music& music, float& volume, bool& muted, bool& godMode, bool& gridVisible);
    ~GameStateManager();


    GameStateManager(const GameStateManager&) = delete;
    GameStateManager& operator=(const GameStateManager&) = delete;


    void updateGameOverScreen();
    void renderGameOverScreen() const;
    void handleGameOverInput();

    void updateIntroScreen();
    void renderIntroScreen() const;
    void handleIntroInput();
    bool isIntroFading() const { return fadingOut; }
    bool isIntroFadeComplete() const { return fadeAlpha <= 0.0f; }


    void updatePauseMenu();
    void renderPauseMenu() const;
    void handlePauseMenuClick(int x, int y);
    void toggleMusic();
    void toggleGodMode();
    void toggleGridMap();
    void setMusicVolume(float volume);


    void updateCompletionScreen();
    void renderCompletionScreen() const;
    void handleCompletionInput();


    void setEngineReference(GameEngine* engine) { engineRef = engine; }

    void initGameOverScreen();
};

#endif