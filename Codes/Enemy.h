#ifndef ENEMY_H
#define ENEMY_H

#include "Entity.h"

/**
 * Enemy class representing moving obstacles
 * Moves randomly within the maze
 */
class Enemy : public Entity {
private:
    int moveTimer;     // Timer for movement frequency
    int moveDelay;     // Delay between moves

public:
    // Constructor
    Enemy(int startX, int startY, int delay = 3);

    // Destructor
    ~Enemy() override = default;

    // Movement
    void moveRandomly(const char maze[10][10]);
    bool canMoveTo(int newX, int newY, const char maze[10][10]) const;

    // Override virtual methods
    void update() override;
    void draw() const override;

    // Utility
    void resetTimer() { moveTimer = 0; }
};

#endif // ENEMY_H
