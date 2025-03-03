#include "SoundManager.h"
#include <iostream>

SoundManager* SoundManager::instance = nullptr;

SoundManager& SoundManager::getInstance() {
    if (!instance) {
        instance = new SoundManager();
    }
    return *instance;
}

void SoundManager::loadSound(const std::string& name, const std::string& filepath) {

    if (buffers.find(name) == buffers.end()) {
        auto buffer = std::make_unique<sf::SoundBuffer>();
        if (buffer->loadFromFile(filepath)) {
            buffers[name] = std::move(buffer);
            sounds[name] = std::make_unique<sf::Sound>(*buffers[name]);

            sounds[name]->setVolume(volume);
        } else {
            std::cerr << "fail " << filepath << std::endl;
        }
    }
}

void SoundManager::playSound(const std::string& name) {
    auto it = sounds.find(name);
    if (it != sounds.end()) {

        it->second->setVolume(volume);
        it->second->play();
    } else {
        std::cerr << "fail " << name << std::endl;
    }
}