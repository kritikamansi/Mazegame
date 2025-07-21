#ifndef ENTITY_H
#define ENTITY_H

/**
 * Base class for all game entities (Player, Enemy, Collectible)
 * Provides common functionality for position and display
 */
class Entity {
protected:
    int x, y;          // Position coordinates
    char symbol;       // Display character
    bool active;       // Whether entity is active in game

public:
    // Constructor
    Entity(int startX, int startY, char displaySymbol);

    // Virtual destructor for proper inheritance
    virtual ~Entity() = default;

    // Getters
    int getX() const { return x; }
    int getY() const { return y; }
    char getSymbol() const { return symbol; }
    bool isActive() const { return active; }

    // Setters
    void setPosition(int newX, int newY);
    void setActive(bool state) { active = state; }

    // Virtual methods for derived classes
    virtual void update() = 0;  // Pure virtual - must be implemented
    virtual void draw() const;  // Can be overridden
};

#endif // ENTITY_H
