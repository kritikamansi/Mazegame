#include "Player.h"
#include <iostream>

/**
 * Constructor for Player class
 * @param startX Initial X position
 * @param startY Initial Y position
 */
Player::Player(int startX, int startY)
    : Entity(startX, startY, 'P'), score(0), moveCount(0) {
}

/**
 * Move the player by specified delta values
 * @param deltaX Change in X position
 * @param deltaY Change in Y position
 */
void Player::move(int deltaX, int deltaY) {
    x += deltaX;
    y += deltaY;
    incrementMoveCount();
}

/**
 * Add points to the player's score
 * @param points Points to add
 */
void Player::addScore(int points) {
    score += points;
}

/**
 * Set the player's score directly
 * @param newScore New score value
 */
void Player::setScore(int newScore) {
    score = newScore;
}

/**
 * Increment the move counter
 */
void Player::incrementMoveCount() {
    moveCount++;
}

/**
 * Set the move count directly
 * @param newMoves New move count value
 */
void Player::setMoveCount(int newMoves) {
    moveCount = newMoves;
}

/**
 * Reset player statistics (score and move count)
 */
void Player::resetStats() {
    score = 0;
    moveCount = 0;
}

/**
 * Update player state (called each game loop)
 * Currently no special update logic needed for player
 */
void Player::update() {
    // Player update logic can be added here if needed
    // For now, player is updated through direct movement
}

/**
 * Draw the player character
 */
void Player::draw() const {
    if (active) {
        std::cout << symbol;
    }
}

/**
 * Check if player can move to a specific position
 * @param newX Target X position
 * @param newY Target Y position
 * @param maze The maze grid to check against
 * @return true if movement is valid, false otherwise
 */
bool Player::canMoveTo(int newX, int newY, const char maze[10][10]) const {
    // Check bounds
    if (newX < 0 || newX >= 10 || newY < 0 || newY >= 10) {
        return false;
    }

    // Check if target position is not a wall
    return maze[newY][newX] != '#';
}
