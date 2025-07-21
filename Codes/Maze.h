#ifndef MAZE_H
#define MAZE_H

#include <vector>
#include <string>

/**
 * Maze class managing the 2D grid layout and display
 * Handles file I/O for save/load functionality
 */
class Maze {
private:
    static const int MAZE_SIZE = 10;
    char grid[MAZE_SIZE][MAZE_SIZE];
    int exitX, exitY;  // Exit position

public:
    // Constructor
    Maze();

    // Destructor
    ~Maze() = default;

    // Grid management
    void initializeDefaultMaze();
    void initializeMaze(int level);
    void display() const;
    void clearScreen() const;

    // File I/O
    bool saveToFile(const std::string& filename) const;
    bool loadFromFile(const std::string& filename);

    // Grid access
    char getCell(int x, int y) const;
    void setCell(int x, int y, char value);
    bool isValidPosition(int x, int y) const;
    bool isWall(int x, int y) const;
    bool isExit(int x, int y) const;

    // Utility
    void getExitPosition(int& x, int& y) const;
    static int getMazeSize() { return MAZE_SIZE; }

    // Level generation
    void generateLevel(int levelNumber);
    void addWalls(int density);
    void ensurePathExists();
};

#endif // MAZE_H
