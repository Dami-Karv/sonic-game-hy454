cmake_minimum_required(VERSION 3.27)
project(HY454_Project VERSION 1.0 DESCRIPTION "Sonic Game Project for HY454" LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

include(FetchContent)
FetchContent_Declare(SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 3.0.0
        GIT_SHALLOW ON
        EXCLUDE_FROM_ALL
        SYSTEM
)
FetchContent_MakeAvailable(SFML)

set(SOURCES
        src/main.cpp src/GameMap.cpp src/GameEngine.cpp src/Player.cpp
        src/GameStateManager.cpp src/FlowerSprite.cpp src/BaseSprite.cpp
        src/BridgeSprite.cpp src/RingSprite.cpp src/SpikeSprite.cpp
        src/CheckpointSprite.cpp src/BuzzerEnemy.cpp src/MotobugEnemy.cpp
        src/CrabmeatEnemy.cpp src/FishEnemy.cpp src/PowerUpSprite.cpp
        src/powerup_effects.cpp src/PlatformSprite.cpp src/SpringSprite.cpp
        src/AnimalSprite.cpp src/SoundManager.cpp
)

set(HEADERS
        include/GameEngine.h include/GameMap.h include/Player.h
        include/GameStateManager.h include/FlowerSprite.h include/GameState.h
        include/BaseSprite.h include/BridgeSprite.h include/RingSprite.h
        include/SpikeSprite.h include/CheckpointSprite.h include/BuzzerEnemy.h
        include/MotobugEnemy.h include/CrabmeatEnemy.h include/FishEnemy.h
        include/PowerUpSprite.h include/powerup_effects.h include/PlatformSprite.h
        include/SpringSprite.h include/AnimalSprite.h include/SoundManager.h
)


add_executable(main ${SOURCES} ${HEADERS})
target_include_directories(main PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(main PRIVATE SFML::Graphics SFML::Audio SFML::System)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
target_compile_definitions(main PRIVATE SFML_STATIC)
target_compile_options(main PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/W4>
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wextra -Wpedantic>)

add_custom_command(
        TARGET main POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/assets ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets
        COMMENT "Copying assets..."
)

install(TARGETS main RUNTIME DESTINATION bin)
install(DIRECTORY assets/ DESTINATION bin/assets)


message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Source dir: ${CMAKE_SOURCE_DIR}")
message(STATUS "Binary dir: ${CMAKE_BINARY_DIR}")