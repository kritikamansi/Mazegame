#include "Collectible.h"
#include <iostream>

/**
 * Constructor for Collectible class
 * @param startX Initial X position
 * @param startY Initial Y position
 * @param points Point value when collected
 */
Collectible::Collectible(int startX, int startY, int points)
    : Entity(startX, startY, '*'), pointValue(points), collected(false) {
}

/**
 * Collect this item (mark as collected and deactivate)
 */
void Collectible::collect() {
    collected = true;
    active = false;
}

/**
 * Reset the collectible to its initial state
 */
void Collectible::reset() {
    collected = false;
    active = true;
}

/**
 * Update collectible state (called each game loop)
 * Currently no special update logic needed for collectibles
 */
void Collectible::update() {
    // Collectibles don't need active updates
    // They are passive until collected
}

/**
 * Draw the collectible item
 * Only draws if not collected
 */
void Collectible::draw() const {
    if (active && !collected) {
        std::cout << symbol;
    }
}
