#ifndef GAME_H
#define GAME_H

#include "Maze.h"
#include "Player.h"
#include "Enemy.h"
#include "Collectible.h"
#include <vector>
#include <memory>

/**
 * Game class managing overall game state and logic
 * Handles game loop, input, collision detection, and level progression
 */
class Game {
private:
    Maze maze;
    Player player;
    std::vector<std::unique_ptr<Enemy>> enemies;
    std::vector<std::unique_ptr<Collectible>> collectibles;

    int currentLevel;
    bool gameRunning;
    bool gameWon;
    bool gameOver;
    bool needsRedraw;

    // Game constants
    static const int MAX_LEVELS = 5;

public:
    // Constructor
    Game();

    // Destructor
    ~Game() = default;

    // Game loop
    void run();
    void update();
    void render();

    // Input handling
    void handleInput();
    char getInput() const;

    // Game state management
    void initializeLevel(int level);
    void nextLevel();
    void resetGame();
    void checkGameOver();
    void checkWinCondition();

    // Collision detection
    void checkCollisions();
    void checkEnemyCollisions();
    void checkCollectibleCollisions();

    // Game mechanics
    void movePlayer(int deltaX, int deltaY);
    bool isValidPlayerMove(int newX, int newY) const;

    // Display
    void displayMenu() const;
    void displaySaveMenu();
    void displayLoadMenu();
    bool showStartupMenu();

    // File operations
    void saveGame() const;
    void saveGameToSlot(int slot) const;
    void loadGame();
    void loadGameFromSlot(int slot);
    void waitForKeyPress() const;

    // Utility
    bool isGameRunning() const { return gameRunning; }
    int getCurrentLevel() const { return currentLevel; }
};

#endif // GAME_H
