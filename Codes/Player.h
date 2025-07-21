#ifndef PLAYER_H
#define PLAYER_H

#include "Entity.h"

/**
 * Player class representing the game player
 * Handles movement and score tracking
 */
class Player : public Entity {
private:
    int score;         // Player's current score
    int moveCount;     // Number of moves made

public:
    // Constructor
    Player(int startX, int startY);

    // Destructor
    ~Player() override = default;

    // Getters
    int getScore() const { return score; }
    int getMoveCount() const { return moveCount; }

    // Game mechanics
    void move(int deltaX, int deltaY);
    void addScore(int points);
    void setScore(int newScore);
    void incrementMoveCount();
    void setMoveCount(int newMoves);
    void resetStats();

    // Override virtual methods
    void update() override;
    void draw() const override;

    // Movement validation
    bool canMoveTo(int newX, int newY, const char maze[10][10]) const;
};

#endif // PLAYER_H
