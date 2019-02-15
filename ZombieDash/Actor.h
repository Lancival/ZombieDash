#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

class StudentWorld;

/* Actor Class Declaration
 * - Base class for all actors in Zombie Dash
 * - Abstract class but only doSomething() is pure virtual
 * - Contains default values for various properties, but can be overriden by derived classes
 */
class Actor : public GraphObject {
public:
    // Constructor
    Actor(int imageID, int startX, int startY, Direction startDirection, int depth, StudentWorld* stWorld);
    
    // Properties
    virtual bool flammable() const; // Default of true (can be destroyed by flames)
    virtual bool infectable() const; // Default of false (cannot be infected by vomit)
    virtual bool blocksMovement() const; // Default of false (blocks other actors from moving onto it)
    virtual bool blocksProjectiles() const; // Default of false (blocks flames and vomit)
    
    // Accessors
    bool alive() const; // Returns whether this actor is alive
    
    // Actions
    void setDead(); // Directly sets state of actor to dead
    virtual void destroy(); // Destroys actor as if by destroyed by fire
    virtual void doSomething() = 0;
protected:
    // Accessors
    StudentWorld* world() const; // Returns a pointer to the Studentworld that "contains" this actor
private:
    bool m_alive; // Whether this actor is alive
    StudentWorld* m_world; // Pointer to Studentworld that "contains" this actor
};

/* Terrain Class Declaration
 * - Base class for walls, exits, and pits
 * - All terrain objects are not flammable and are drawn with an orientation of right
 */
class Terrain : public Actor {
public:
    // Constructor
    Terrain(int imageID, int startX, int startY, int depth, StudentWorld* stWorld);
    
    // Properties
    virtual bool flammable() const; // Terrain objects are not flammable
};

// Wall Class Declaration
class Wall : public Terrain {
public:
    // Constructor
    Wall(int startX, int startY, StudentWorld* stWorld);
    
    // Properties
    virtual bool blocksMovement() const; // Walls do block movement
    virtual bool blocksProjectiles() const; // Walls do block projectiles
    
    // Actions
    virtual void doSomething(); // Do nothing
};

// Exit Class Declaration
class Exit : public Terrain {
public:
    // Constructor
    Exit(int startX, int startY, StudentWorld* stWorld);
    
    // Properties
    virtual bool blocksProjectiles() const; // Exits do block projectiles
    
    // Actions
    virtual void doSomething();
};

// Pit Class Declaration
class Pit : public Terrain {
public:
    // Constructor
    Pit(int startX, int startY, StudentWorld* stWorld);
    
    // Actions
    virtual void doSomething();
};

/* Projectile Class Declaration
 * - Base class for vomit and flames
 * - All projectiles are not flammable, are destroyed after 2 ticks, and are drawn at a depth of 0
 */
class Projectile : public Actor {
public:
    // Constructor
    Projectile(int imageID, int startX, int startY, Direction startDirection, StudentWorld* stWorld);
    
    // Properties
    virtual bool flammable() const; // Projectiles are not flammable
    
    // Actions
    virtual void doSomething(); // Infect or destroy infectable or destructible actors
protected:
    // Accessors
    int ticksLeft() const; // Returns number of ticks left until destruction
    
    // Actions
    void decTicks(); // Decrements number of ticks left until destruction
    virtual void affect() = 0; // Implementation of infection/destruction
private:
    int m_ticksLeft; // Number of ticks left until destruction
};

// Flame Class Declaration
class Flame : public Projectile {
public:
    // Constructor
    Flame(int startX, int startY, Direction startDirection, StudentWorld* stWorld);
protected:
    // Actions
    virtual void affect(); // Destroy destructible actors
};

// Vomit Class Declaration
class Vomit : public Projectile {
public:
    // Constructor
    Vomit(int startX, int startY, Direction startDirection, StudentWorld* stWorld);
protected:
    // Actions
    virtual void affect(); // Infect infectable actors
};

/* Goodie Class Declaration
 * - Base class for vaccine goodies, gas can goodies, and landmine goodies
 * - All goodies are drawn with an orientation of right at a depth of 1
 * - All goodies play the same sound when obtained, and change Penelope's inventory when obtained
 */
class Goodie : public Actor {
public:
    // Constructor
    Goodie(int imageID, int startX, int startY, StudentWorld* stWorld);
    
    // Actions
    virtual void doSomething(); // Detect if goodie was picked up by Penelope
protected:
    // Actions
    virtual void receiveGoodies() = 0; // Increase the appropriate portion of Penelope's inventory
    void playGoodieSound(); // Play SOUND_GOT_GOODIE
};

// VaccineGoodie Class Declaration
class VaccineGoodie : public Goodie {
public:
    // Constructor
    VaccineGoodie(int startX, int startY, StudentWorld* stWorld);
protected:
    // Actions
    virtual void receiveGoodies(); // Increase Penelope's number of vaccines by 1
};

// GasCanGoodie Class Declaration
class GasCanGoodie : public Goodie {
public:
    // Constructor
    GasCanGoodie(int startX, int startY, StudentWorld* stWorld);
protected:
    // Actions
    virtual void receiveGoodies(); // Increase Penelope's number of flamethrower chargers by 5
};

// LandmineGoodie Class Declaration
class LandmineGoodie : public Goodie {
public:
    // Constructor
    LandmineGoodie(int startX, int startY, StudentWorld* stWorld);
protected:
    // Actions
    virtual void receiveGoodies(); // Increase Penelope's number of landmines by 2
};

/* Landmine Class Declaration
 * - Landmines are sufficiently different from other actors to merit not having a base class other than Actor
 * - Landmines are drawn with an orientation of right and a depth of 1
 * - Landmines start with 30 safety ticks before becoming active
 */
class Landmine : public Actor {
public:
    // Constructor
    Landmine(int startX, int startY, StudentWorld* stWorld);
    
    // Accessors
    int safetyTicks() const; // Return the number of safety ticks remaining
    bool active() const; // Return whether the landmine is active
    
    // Actions
    virtual void destroy(); // Sets state to dead and creates flames and a pit
    virtual void doSomething(); // Detects whether the landmine has been stepped upon
private:
    int m_safetyTicks; // Number of safety ticks remaining
    bool m_active; // Whether the landmine is active
};

#endif // ACTOR_H_
