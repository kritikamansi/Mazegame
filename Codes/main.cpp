/**
 * Main entry point for the Maze Game
 *
 * This is a 2D maze game implemented in C++ featuring:
 * - 10x10 grid maze with walls, paths, player, and exit
 * - WASD movement controls with collision detection
 * - Collectible items for scoring
 * - Moving enemies with random AI
 * - Multiple levels with increasing difficulty
 * - Save/load functionality for maze layouts
 * - Object-oriented design with inheritance
 *
 * Author: Kritika
 */

#include "Game.h"
#include <iostream>
#include <cstdlib>
#include <ctime>

int main() {
    try {
        // Seed random number generator
        srand(static_cast<unsigned int>(time(nullptr)));

        // Create and run the game
        Game mazeGame;
        mazeGame.run();

        std::cout << "Thanks for playing!" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "An unknown error occurred!" << std::endl;
        return 1;
    }

    return 0;
}
