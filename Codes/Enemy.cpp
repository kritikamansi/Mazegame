#include "Enemy.h"
#include <iostream>
#include <cstdlib>
#include <ctime>

/**
 * Constructor for Enemy class
 * @param startX Initial X position
 * @param startY Initial Y position
 * @param delay Movement delay (higher = slower movement)
 */
Enemy::Enemy(int startX, int startY, int delay)
    : Entity(startX, startY, 'X'), moveTimer(0), moveDelay(delay) {
    // Seed random number generator
    static bool seeded = false;
    if (!seeded) {
        srand(static_cast<unsigned int>(time(nullptr)));
        seeded = true;
    }
}

/**
 * Move the enemy randomly within the maze
 * @param maze The maze grid to check against
 */
void Enemy::moveRandomly(const char maze[10][10]) {
    // Only move if timer has reached delay
    if (moveTimer < moveDelay) {
        return;
    }

    // Reset timer
    moveTimer = 0;

    // Generate random direction (0=up, 1=right, 2=down, 3=left)
    int direction = rand() % 4;
    int newX = x;
    int newY = y;

    switch (direction) {
        case 0: newY--; break; // Up
        case 1: newX++; break; // Right
        case 2: newY++; break; // Down
        case 3: newX--; break; // Left
    }

    // Check if movement is valid
    if (canMoveTo(newX, newY, maze)) {
        x = newX;
        y = newY;
    }
}

/**
 * Check if enemy can move to a specific position
 * @param newX Target X position
 * @param newY Target Y position
 * @param maze The maze grid to check against
 * @return true if movement is valid, false otherwise
 */
bool Enemy::canMoveTo(int newX, int newY, const char maze[10][10]) const {
    // Check bounds
    if (newX < 0 || newX >= 10 || newY < 0 || newY >= 10) {
        return false;
    }

    // Check if target position is not a wall
    return maze[newY][newX] != '#';
}

/**
 * Update enemy state (called each game loop)
 * Increments movement timer and handles random movement
 */
void Enemy::update() {
    if (active) {
        moveTimer++;
    }
}

/**
 * Draw the enemy character
 */
void Enemy::draw() const {
    if (active) {
        std::cout << symbol;
    }
}
