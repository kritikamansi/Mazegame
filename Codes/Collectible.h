#ifndef COLLECTIBLE_H
#define COLLECTIBLE_H

#include "Entity.h"

/**
 * Collectible class representing items that can be collected for points
 * Provides score bonus when collected by player
 */
class Collectible : public Entity {
private:
    int pointValue;    // Points awarded when collected
    bool collected;    // Whether item has been collected

public:
    // Constructor
    Collectible(int startX, int startY, int points = 10);

    // Destructor
    ~Collectible() override = default;

    // Getters
    int getPointValue() const { return pointValue; }
    bool isCollected() const { return collected; }

    // Collection mechanics
    void collect();
    void reset();

    // Override virtual methods
    void update() override;
    void draw() const override;
};

#endif // COLLECTIBLE_H
