#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H

#include <SFML/Audio.hpp>
#include <memory>
#include <string>
#include <unordered_map>

class SoundManager {
private:
    static SoundManager* instance;
    std::unordered_map<std::string, std::unique_ptr<sf::SoundBuffer>> buffers;
    std::unordered_map<std::string, std::unique_ptr<sf::Sound>> sounds;
    float volume = 30.0f;

    SoundManager() = default;

public:
    static SoundManager& getInstance();
    void loadSound(const std::string& name, const std::string& filepath);
    void playSound(const std::string& name);



    void setVolume(float newVolume) {
        volume = std::clamp(newVolume, 0.0f, 100.0f);

        for (auto& [name, sound] : sounds) {
            if (sound) {
                sound->setVolume(volume);
            }
        }
    }

    float getVolume() const { return volume; }
};

#endif