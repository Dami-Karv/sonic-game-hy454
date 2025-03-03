# Sonic: The CSD Chronicles

A 2D Sonic the Hedgehog game implementation for the HY454 Development of Intelligent Interfaces and Games course (University of Crete, Winter 2024-25)

## Project Overview

This project implements a Sonic the Hedgehog-style platformer game using C++ and SFML, featuring character physics, enemies, power-ups, and level design based on the original Sonic games. The implementation follows the game engine architecture taught in the HY454 course.

## Features

- Tile-based map construction with collision detection
- Parallax scrolling for background layers
- Sonic character with physics-based movement including running, jumping, and rolling
- Multiple enemy types (Masher/Batabata, Crabmeat/Ganigani, Buzz Bomber/Beeton, Motobug/Motora)
- Interactive elements (rings, spikes, springs, checkpoints)
- Power-ups (rings, invincibility, speed boost, shield, health up)
- Rings physics when Sonic takes damage
- Special spin loops for Sonic
- Pause/resume functionality
- Sound effects and background music


## Prerequisites

- C++17 compatible compiler (GCC 9+, Clang 10+, MSVC 19.20+)
- CMake 3.27 or higher
- SFML 3.0.0 library

## Building and Running

### Using CMake

1. Clone the repository:
   ```
   git clone https://github.com/YourUsername/sonic-csd-chronicles.git
   cd sonic-csd-chronicles
   ```

2. Create a build directory and run CMake:
   ```
   mkdir build
   cd build
   cmake ..
   ```

3. Build the project:
   ```
   cmake --build . --config Debug
   ```

4. Run the game:
   ```
   ./bin/main
   ```

## Controls

- **Arrow Keys**: Move Sonic
- **Space**: Jump/Roll
- **ESC**: Pause/Resume game
- **Enter**: Start game (from intro screen)

## Credits

This project was developed as an academic exercise for the HY454 course "Development of Intelligent Interfaces and Games" at the Department of Computer Science, University of Crete.

## License

This project is created for educational purposes only. Sonic the Hedgehog is a registered trademark of SEGA. All game assets, characters, and original concepts belong to their respective owners.
