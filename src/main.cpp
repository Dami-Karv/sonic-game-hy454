#include <iostream>
#include "../include/GameEngine.h"

int main() {
    try {
        auto gameEngine = GameEngine();

        while (gameEngine.running()) {
            gameEngine.update();
            gameEngine.render();
        }
    }
    catch (const std::exception& e) {
        std::cerr << "error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}