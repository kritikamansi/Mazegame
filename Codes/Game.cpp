#include "Game.h"
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <string>
#include <fstream>

#ifdef _WIN32
    #include <conio.h>
    #include <windows.h>
#else
    #include <termios.h>
    #include <unistd.h>
    #include <sys/select.h>
#endif

/**
 * Constructor for Game class
 * Initializes game state and starts at level 1
 */
Game::Game() : player(1, 1), currentLevel(1), gameRunning(true), gameWon(false), gameOver(false), needsRedraw(true) {
    initializeLevel(1);
}

/**
 * Main game loop
 * Handles input, updates game state, and renders
 */
void Game::run() {
    // Show startup menu first
    if (!showStartupMenu()) {
        return; // User chose to quit
    }

    displayMenu();
    needsRedraw = true;
    render(); // Initial render

    while (gameRunning) {
        handleInput();
        update();
        checkCollisions();
        checkWinCondition();
        checkGameOver();

        // Only render when something has changed
        if (needsRedraw) {
            render();
            needsRedraw = false;
        }

        // Optimized refresh rate for best balance of smoothness and flicker reduction
#ifdef _WIN32
        Sleep(120);
#else
        usleep(120000);
#endif
    }
}

/**
 * Update all game entities and state
 */
void Game::update() {
    // Update player
    player.update();

    // Update enemies
    for (auto& enemy : enemies) {
        enemy->update();
        if (enemy->isActive()) {
            // Get maze grid for movement validation
            char mazeGrid[10][10];
            for (int i = 0; i < 10; i++) {
                for (int j = 0; j < 10; j++) {
                    mazeGrid[i][j] = maze.getCell(j, i);
                }
            }
            int oldX = enemy->getX();
            int oldY = enemy->getY();
            enemy->moveRandomly(mazeGrid);
            // Check if enemy actually moved
            if (oldX != enemy->getX() || oldY != enemy->getY()) {
                needsRedraw = true;
            }
        }
    }

    // Update collectibles
    for (auto& collectible : collectibles) {
        collectible->update();
    }
}

/**
 * Render the complete game state
 */
void Game::render() {
    // Build complete frame in string first - simple double buffering
    std::string frame = "";

    // Create display grid
    char displayGrid[10][10];
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            displayGrid[i][j] = maze.getCell(j, i);
        }
    }

    // Place collectibles
    for (const auto& collectible : collectibles) {
        if (collectible->isActive() && !collectible->isCollected()) {
            displayGrid[collectible->getY()][collectible->getX()] = collectible->getSymbol();
        }
    }

    // Place enemies
    for (const auto& enemy : enemies) {
        if (enemy->isActive()) {
            displayGrid[enemy->getY()][enemy->getX()] = enemy->getSymbol();
        }
    }

    // Place player
    if (player.isActive()) {
        displayGrid[player.getY()][player.getX()] = player.getSymbol();
    }

    // Build maze display
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            frame += displayGrid[i][j];
            frame += " ";
        }
        frame += "\n";
    }

    // Add game info
    frame += "\n";
    frame += "Level: " + std::to_string(currentLevel) +
             " | Score: " + std::to_string(player.getScore()) +
             " | Moves: " + std::to_string(player.getMoveCount()) + "\n";
    frame += "Controls: WASD to move, Q to quit, R to restart, P to save menu, L to load menu\n";

    if (gameWon && currentLevel < MAX_LEVELS) {
        frame += "Level Complete! Press N for next level\n";
    }

    if (gameOver) {
        frame += "\nGAME OVER! You collided with an enemy!\n";
        frame += "Final Score: " + std::to_string(player.getScore()) + "\n";
    } else if (gameWon && currentLevel >= MAX_LEVELS) {
        frame += "\nCONGRATULATIONS! You completed all levels!\n";
        frame += "Final Score: " + std::to_string(player.getScore()) + "\n";
        frame += "Total Moves: " + std::to_string(player.getMoveCount()) + "\n";
    }

    // Clear and display everything at once - minimal flicker
    maze.clearScreen();
    std::cout << frame;
    std::cout.flush();
}

/**
 * Handle player input
 */
void Game::handleInput() {
    char input = getInput();

    switch (input) {
        case 'w': case 'W':
            movePlayer(0, -1);
            break;
        case 's': case 'S':
            movePlayer(0, 1);
            break;
        case 'a': case 'A':
            movePlayer(-1, 0);
            break;
        case 'd': case 'D':
            movePlayer(1, 0);
            break;
        case 'q': case 'Q':
            gameRunning = false;
            break;
        case 'r': case 'R':
            resetGame();
            needsRedraw = true;
            break;
        case 'n': case 'N':
            if (gameWon && currentLevel < MAX_LEVELS) {
                nextLevel();
                needsRedraw = true;
            }
            break;
        case 'l': case 'L':
            displayLoadMenu();
            needsRedraw = true;
            break;
        case 'p': case 'P':
            displaySaveMenu();
            needsRedraw = true;
            break;
    }
}

/**
 * Get character input from user
 * @return Character entered by user
 */
char Game::getInput() const {
#ifdef _WIN32
    if (_kbhit()) {
        return _getch();
    }
    return 0;
#else
    // Non-blocking input for Unix-like systems
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(STDIN_FILENO, &readfds);

    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 1000; // 1ms timeout

    char ch = 0;
    if (select(STDIN_FILENO + 1, &readfds, NULL, NULL, &timeout) > 0) {
        read(STDIN_FILENO, &ch, 1);
    }

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
#endif
}

/**
 * Initialize a specific level
 * @param level Level number to initialize
 */
void Game::initializeLevel(int level) {
    currentLevel = level;
    gameWon = false;
    gameOver = false;
    needsRedraw = true;

    // Initialize maze for level
    maze.initializeMaze(level);

    // Reset player position
    player.setPosition(1, 1);
    player.setActive(true);

    // Clear existing entities
    enemies.clear();
    collectibles.clear();

    // Add enemies based on level (more enemies on higher levels)
    int enemyCount = std::min(level, 3);
    for (int i = 0; i < enemyCount; i++) {
        int x, y;
        do {
            x = rand() % 8 + 1; // Avoid borders
            y = rand() % 8 + 1;
        } while (maze.isWall(x, y) || (x == 1 && y == 1) || maze.isExit(x, y));

        enemies.push_back(std::unique_ptr<Enemy>(new Enemy(x, y, 3 + level)));
    }

    // Add collectibles (more on higher levels)
    int collectibleCount = 3 + level;
    for (int i = 0; i < collectibleCount; i++) {
        int x, y;
        do {
            x = rand() % 8 + 1;
            y = rand() % 8 + 1;
        } while (maze.isWall(x, y) || (x == 1 && y == 1) || maze.isExit(x, y));

        collectibles.push_back(std::unique_ptr<Collectible>(new Collectible(x, y, 10 * level)));
    }
}

/**
 * Move to next level
 */
void Game::nextLevel() {
    if (currentLevel < MAX_LEVELS) {
        initializeLevel(currentLevel + 1);
        needsRedraw = true;
    }
}

/**
 * Reset the current game
 */
void Game::resetGame() {
    player.resetStats();
    initializeLevel(1);
    needsRedraw = true;
}

/**
 * Check for game over conditions
 */
void Game::checkGameOver() {
    if (gameOver || gameWon) {
        return;
    }

    // Game over is handled in collision detection
}

/**
 * Check for win conditions
 */
void Game::checkWinCondition() {
    if (gameOver || gameWon) {
        return;
    }

    // Check if player reached exit
    if (maze.isExit(player.getX(), player.getY())) {
        if (currentLevel >= MAX_LEVELS) {
            gameWon = true;
            gameRunning = false;
            needsRedraw = true;
        } else {
            gameWon = true; // Won this level
            needsRedraw = true;
        }
    }
}

/**
 * Check all collision types
 */
void Game::checkCollisions() {
    checkEnemyCollisions();
    checkCollectibleCollisions();
}

/**
 * Check collisions with enemies
 */
void Game::checkEnemyCollisions() {
    for (const auto& enemy : enemies) {
        if (enemy->isActive() &&
            enemy->getX() == player.getX() &&
            enemy->getY() == player.getY()) {
            gameOver = true;
            gameRunning = false;
            needsRedraw = true;
            return;
        }
    }
}

/**
 * Check collisions with collectibles
 */
void Game::checkCollectibleCollisions() {
    for (auto& collectible : collectibles) {
        if (collectible->isActive() &&
            !collectible->isCollected() &&
            collectible->getX() == player.getX() &&
            collectible->getY() == player.getY()) {

            collectible->collect();
            player.addScore(collectible->getPointValue());
            needsRedraw = true;
        }
    }
}

/**
 * Move player in specified direction
 * @param deltaX Change in X position
 * @param deltaY Change in Y position
 */
void Game::movePlayer(int deltaX, int deltaY) {
    int newX = player.getX() + deltaX;
    int newY = player.getY() + deltaY;

    if (isValidPlayerMove(newX, newY)) {
        player.move(deltaX, deltaY);
        needsRedraw = true;
    }
}

/**
 * Check if player movement is valid
 * @param newX Target X position
 * @param newY Target Y position
 * @return true if movement is valid
 */
bool Game::isValidPlayerMove(int newX, int newY) const {
    // Check bounds
    if (newX < 0 || newX >= 10 || newY < 0 || newY >= 10) {
        return false;
    }

    // Check for walls
    return !maze.isWall(newX, newY);
}



/**
 * Display game menu
 */
void Game::displayMenu() const {
    maze.clearScreen();
    std::cout << "=== MAZE GAME ===" << std::endl;
    std::cout << "Navigate through the maze to reach the exit (E)" << std::endl;
    std::cout << "Collect items (*) for points" << std::endl;
    std::cout << "Avoid enemies (X)" << std::endl;
    std::cout << std::endl;
    std::cout << "Legend:" << std::endl;
    std::cout << "# = Wall" << std::endl;
    std::cout << "P = Player" << std::endl;
    std::cout << "E = Exit" << std::endl;
    std::cout << "* = Collectible" << std::endl;
    std::cout << "X = Enemy" << std::endl;
    std::cout << std::endl;
    std::cout << "Press any key to start..." << std::endl;

#ifdef _WIN32
    _getch();
#else
    getchar();
#endif
}

/**
 * Save current game state to file
 */
void Game::saveGame() const {
    if (maze.saveToFile("maze_save.txt")) {
        std::cout << "Game saved successfully!" << std::endl;
    } else {
        std::cout << "Failed to save game!" << std::endl;
    }
}

/**
 * Load game state from file
 */
void Game::loadGame() {
    if (maze.loadFromFile("maze_save.txt")) {
        std::cout << "Game loaded successfully!" << std::endl;
    } else {
        std::cout << "Failed to load game!" << std::endl;
    }
}

/**
 * Save complete game state to a specific slot
 */
void Game::saveGameToSlot(int slot) const {
    std::string filename = "maze_save_" + std::to_string(slot) + ".txt";
    std::ofstream file(filename);

    if (!file.is_open()) {
        std::cout << "Failed to save game to slot " << slot << "!" << std::endl;
        return;
    }

    // Save game state
    file << "MAZEGAME_SAVE_V1" << std::endl;  // File format identifier
    file << currentLevel << std::endl;         // Current level
    file << player.getScore() << std::endl;    // Player score
    file << player.getMoveCount() << std::endl; // Player moves
    file << player.getX() << " " << player.getY() << std::endl; // Player position

    // Save maze layout
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            file << maze.getCell(j, i);
        }
        file << std::endl;
    }

    // Save exit position
    int exitX, exitY;
    maze.getExitPosition(exitX, exitY);
    file << exitX << " " << exitY << std::endl;

    file.close();
    std::cout << "Complete game state saved to slot " << slot << " successfully!" << std::endl;
}

/**
 * Load complete game state from a specific slot
 */
void Game::loadGameFromSlot(int slot) {
    std::string filename = "maze_save_" + std::to_string(slot) + ".txt";
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cout << "Failed to load game from slot " << slot << "! File may not exist." << std::endl;
        return;
    }

    std::string fileFormat;
    file >> fileFormat;

    if (fileFormat != "MAZEGAME_SAVE_V1") {
        std::cout << "Invalid save file format!" << std::endl;
        file.close();
        return;
    }

    // Load game state
    int savedLevel, savedScore, savedMoves, playerX, playerY;
    file >> savedLevel >> savedScore >> savedMoves >> playerX >> playerY;

    // Load maze layout
    char loadedMaze[10][10];
    std::string line;
    std::getline(file, line); // consume newline after coordinates

    for (int i = 0; i < 10; i++) {
        std::getline(file, line);
        if (line.length() >= 10) {
            for (int j = 0; j < 10; j++) {
                loadedMaze[i][j] = line[j];
            }
        }
    }

    // Load exit position
    int exitX, exitY;
    file >> exitX >> exitY;
    file.close();

    // Apply loaded state
    currentLevel = savedLevel;

    // Update maze with loaded layout
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            maze.setCell(j, i, loadedMaze[i][j]);
        }
    }

    // Set player position and stats
    player.setPosition(playerX, playerY);
    player.resetStats();
    player.setScore(savedScore);
    player.setMoveCount(savedMoves);

    // Clear and regenerate entities for current level
    enemies.clear();
    collectibles.clear();

    // Regenerate enemies and collectibles based on current level
    int enemyCount = std::min(currentLevel, 3);
    for (int i = 0; i < enemyCount; i++) {
        int x, y;
        do {
            x = rand() % 8 + 1;
            y = rand() % 8 + 1;
        } while (maze.isWall(x, y) || (x == playerX && y == playerY) || maze.isExit(x, y));

        enemies.push_back(std::unique_ptr<Enemy>(new Enemy(x, y, 3 + currentLevel)));
    }

    int collectibleCount = 3 + currentLevel;
    for (int i = 0; i < collectibleCount; i++) {
        int x, y;
        do {
            x = rand() % 8 + 1;
            y = rand() % 8 + 1;
        } while (maze.isWall(x, y) || (x == playerX && y == playerY) || maze.isExit(x, y));

        collectibles.push_back(std::unique_ptr<Collectible>(new Collectible(x, y, 10 * currentLevel)));
    }

    std::cout << "Complete game state loaded from slot " << slot << " successfully!" << std::endl;
    std::cout << "Level: " << currentLevel << " | Score: " << savedScore << " | Moves: " << savedMoves << std::endl;
}

/**
 * Wait for any key press - works on both Windows and Unix
 */
void Game::waitForKeyPress() const {
#ifdef _WIN32
    _getch(); // Windows - simple key press
#else
    // Unix/Linux - more complex but reliable
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
#endif
}

/**
 * Show startup menu with load options
 */
bool Game::showStartupMenu() {
    maze.clearScreen();
    std::cout << "=== MAZE GAME - STARTUP ===" << std::endl;
    std::cout << "1. Start New Game" << std::endl;
    std::cout << "2. Load Saved Game" << std::endl;
    std::cout << "3. Quit" << std::endl;
    std::cout << std::endl;
    std::cout << "Choose an option (1-3): ";

    char choice;
    std::cin >> choice;

    switch (choice) {
        case '1':
            return true; // Start new game
        case '2':
            maze.clearScreen();
            std::cout << "=== LOAD GAME ===" << std::endl;
            std::cout << "Choose save slot (1-5): ";
            char slot;
            std::cin >> slot;
            if (slot >= '1' && slot <= '5') {
                loadGameFromSlot(slot - '0');
            } else {
                std::cout << "Invalid slot!" << std::endl;
            }
            std::cout << "Press any key to continue...";
            waitForKeyPress();
            return true;
        case '3':
            return false; // Quit
        default:
            std::cout << "Invalid choice! Starting new game..." << std::endl;
            return true;
    }
}

/**
 * Display save menu during gameplay
 */
void Game::displaySaveMenu() {
    maze.clearScreen();
    std::cout << "=== SAVE GAME ===" << std::endl;
    std::cout << "Choose save slot (1-5): ";

    char slot;
    std::cin >> slot;

    if (slot >= '1' && slot <= '5') {
        saveGameToSlot(slot - '0');
    } else {
        std::cout << "Invalid slot!" << std::endl;
        }

    std::cout << "Press any key to continue...";
    waitForKeyPress();
}

/**
 * Display load menu during gameplay
 */
void Game::displayLoadMenu() {
    maze.clearScreen();
    std::cout << "=== LOAD GAME ===" << std::endl;
    std::cout << "Choose save slot (1-5): ";

    char slot;
    std::cin >> slot;

    if (slot >= '1' && slot <= '5') {
        loadGameFromSlot(slot - '0');
    } else {
        std::cout << "Invalid slot!" << std::endl;
    }

    std::cout << "Press any key to continue...";
    waitForKeyPress();
}






