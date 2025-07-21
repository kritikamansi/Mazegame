#include "Entity.h"
#include <iostream>

/**
 * Constructor for Entity base class
 * @param startX Initial X position
 * @param startY Initial Y position
 * @param displaySymbol Character to display for this entity
 */
Entity::Entity(int startX, int startY, char displaySymbol)
    : x(startX), y(startY), symbol(displaySymbol), active(true) {
}

/**
 * Set the position of the entity
 * @param newX New X coordinate
 * @param newY New Y coordinate
 */
void Entity::setPosition(int newX, int newY) {
    x = newX;
    y = newY;
}

/**
 * Default draw method - can be overridden by derived classes
 * Displays the entity's symbol at its current position
 */
void Entity::draw() const {
    if (active) {
        std::cout << symbol;
    }
}
