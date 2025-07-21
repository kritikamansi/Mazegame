#include "Maze.h"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>

#ifdef _WIN32
    #include <windows.h>
    #include <conio.h>
#else
    #include <cstdlib>
#endif

/**
 * Constructor for Maze class
 * Initializes the maze with default layout
 */
Maze::Maze() : exitX(8), exitY(8) {
    initializeDefaultMaze();
}

/**
 * Initialize the maze with a default layout
 */
void Maze::initializeDefaultMaze() {
    // Create a simple default maze layout
    char defaultMaze[MAZE_SIZE][MAZE_SIZE] = {
        {'#', '#', '#', '#', '#', '#', '#', '#', '#', '#'},
        {'#', ' ', ' ', ' ', '#', ' ', ' ', ' ', ' ', '#'},
        {'#', ' ', '#', ' ', '#', ' ', '#', '#', ' ', '#'},
        {'#', ' ', '#', ' ', ' ', ' ', ' ', '#', ' ', '#'},
        {'#', ' ', '#', '#', '#', '#', ' ', '#', ' ', '#'},
        {'#', ' ', ' ', ' ', ' ', ' ', ' ', '#', ' ', '#'},
        {'#', '#', '#', ' ', '#', ' ', '#', '#', ' ', '#'},
        {'#', ' ', ' ', ' ', '#', ' ', ' ', ' ', ' ', '#'},
        {'#', ' ', '#', '#', '#', '#', '#', '#', 'E', '#'},
        {'#', '#', '#', '#', '#', '#', '#', '#', '#', '#'}
    };

    // Copy default maze to grid
    for (int i = 0; i < MAZE_SIZE; i++) {
        for (int j = 0; j < MAZE_SIZE; j++) {
            grid[i][j] = defaultMaze[i][j];
        }
    }

    exitX = 8;
    exitY = 8;
}

/**
 * Initialize maze for a specific level
 * @param level Level number to generate
 */
void Maze::initializeMaze(int level) {
    if (level == 1) {
        initializeDefaultMaze();
    } else {
        generateLevel(level);
    }
}

/**
 * Display the complete maze to console
 */
void Maze::display() const {
    for (int i = 0; i < MAZE_SIZE; i++) {
        for (int j = 0; j < MAZE_SIZE; j++) {
            std::cout << grid[i][j] << " ";
        }
        std::cout << std::endl;
    }
}

/**
 * Clear the console screen
 */
void Maze::clearScreen() const {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

/**
 * Save maze layout to file
 * @param filename Name of file to save to
 * @return true if successful, false otherwise
 */
bool Maze::saveToFile(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    // Save maze dimensions
    file << MAZE_SIZE << " " << MAZE_SIZE << std::endl;

    // Save exit position
    file << exitX << " " << exitY << std::endl;

    // Save maze grid
    for (int i = 0; i < MAZE_SIZE; i++) {
        for (int j = 0; j < MAZE_SIZE; j++) {
            file << grid[i][j];
        }
        file << std::endl;
    }

    file.close();
    return true;
}

/**
 * Load maze layout from file
 * @param filename Name of file to load from
 * @return true if successful, false otherwise
 */
bool Maze::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    int width, height;
    file >> width >> height;

    // Check if dimensions match
    if (width != MAZE_SIZE || height != MAZE_SIZE) {
        file.close();
        return false;
    }

    // Load exit position
    file >> exitX >> exitY;

    // Load maze grid
    std::string line;
    std::getline(file, line); // consume newline

    for (int i = 0; i < MAZE_SIZE; i++) {
        std::getline(file, line);
        if (line.length() >= MAZE_SIZE) {
            for (int j = 0; j < MAZE_SIZE; j++) {
                grid[i][j] = line[j];
            }
        }
    }

    file.close();
    return true;
}

/**
 * Get cell value at specified position
 * @param x X coordinate
 * @param y Y coordinate
 * @return Character at position
 */
char Maze::getCell(int x, int y) const {
    if (isValidPosition(x, y)) {
        return grid[y][x];
    }
    return '#'; // Return wall for invalid positions
}

/**
 * Set cell value at specified position
 * @param x X coordinate
 * @param y Y coordinate
 * @param value Character to set
 */
void Maze::setCell(int x, int y, char value) {
    if (isValidPosition(x, y)) {
        grid[y][x] = value;
    }
}

/**
 * Check if position is within maze bounds
 * @param x X coordinate
 * @param y Y coordinate
 * @return true if valid, false otherwise
 */
bool Maze::isValidPosition(int x, int y) const {
    return x >= 0 && x < MAZE_SIZE && y >= 0 && y < MAZE_SIZE;
}

/**
 * Check if position contains a wall
 * @param x X coordinate
 * @param y Y coordinate
 * @return true if wall, false otherwise
 */
bool Maze::isWall(int x, int y) const {
    return getCell(x, y) == '#';
}

/**
 * Check if position is the exit
 * @param x X coordinate
 * @param y Y coordinate
 * @return true if exit, false otherwise
 */
bool Maze::isExit(int x, int y) const {
    return x == exitX && y == exitY;
}

/**
 * Get exit position
 * @param x Reference to store exit X coordinate
 * @param y Reference to store exit Y coordinate
 */
void Maze::getExitPosition(int& x, int& y) const {
    x = exitX;
    y = exitY;
}

/**
 * Generate a level with increasing difficulty
 * @param levelNumber Level to generate (higher = more difficult)
 */
void Maze::generateLevel(int levelNumber) {
    // Start with empty maze
    for (int i = 0; i < MAZE_SIZE; i++) {
        for (int j = 0; j < MAZE_SIZE; j++) {
            if (i == 0 || i == MAZE_SIZE-1 || j == 0 || j == MAZE_SIZE-1) {
                grid[i][j] = '#'; // Border walls
            } else {
                grid[i][j] = ' '; // Empty space
            }
        }
    }

    // Add walls based on level difficulty
    int wallDensity = 10 + (levelNumber * 5); // Increase wall density per level
    addWalls(wallDensity);

    // Set exit position
    exitX = MAZE_SIZE - 2;
    exitY = MAZE_SIZE - 2;
    grid[exitY][exitX] = 'E';

    // Ensure there's always a path from start to exit
    ensurePathExists();
}

/**
 * Add random walls to the maze
 * @param density Number of walls to add
 */
void Maze::addWalls(int density) {
    static bool seeded = false;
    if (!seeded) {
        srand(static_cast<unsigned int>(time(nullptr)));
        seeded = true;
    }

    for (int i = 0; i < density; i++) {
        int x = rand() % (MAZE_SIZE - 2) + 1; // Avoid borders
        int y = rand() % (MAZE_SIZE - 2) + 1;

        // Don't place walls on start or exit positions
        if ((x == 1 && y == 1) || (x == exitX && y == exitY)) {
            continue;
        }

        grid[y][x] = '#';
    }
}

/**
 * Ensure there's always a basic path from start to exit
 * Creates a simple L-shaped path as fallback
 */
void Maze::ensurePathExists() {
    // Create a simple path from (1,1) to exit
    // Horizontal path first
    for (int x = 1; x <= exitX; x++) {
        if (grid[1][x] == '#' && !(x == exitX && 1 == exitY)) {
            grid[1][x] = ' ';
        }
    }

    // Vertical path
    for (int y = 1; y <= exitY; y++) {
        if (grid[y][exitX] == '#' && y != exitY) {
            grid[y][exitX] = ' ';
        }
    }
}
