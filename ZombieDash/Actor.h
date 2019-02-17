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
    virtual bool pitDestructible() const; // Default of false (cannot be destroyed by a pit)
    
    // Accessors
    bool alive() const; // Returns whether this actor is alive
    
    // Actions
    void setDead(); // Directly sets state of actor to dead
    virtual void destroy(); // Destroys actor as if by destroyed by fire
    virtual void doSomething() = 0;
    virtual void infect();
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

/* Person Class Declaration
 * - Base class for Penelope, citizens, and zombies
 * - All people block the movement of other people/zombies
 * - All people can fall into pits and be destroyed
 */
class Person : public Actor {
public:
    // Constructor
    Person(int imageID, int startX, int startY, StudentWorld* stWorld, int sound_infect, int sound_flame, int score_value);
    
    // Properties
    virtual bool blocksMovement() const; // People block movement
    virtual bool pitDestructible() const; // People can be destroyed by pits
    
    // Accessors
    int infection() const; // Returns number of ticks this person has been infected
    
    // Actions
    virtual void doSomething(); // Checks if turned into zombie, then call doAction()
    virtual void destroy(); // People play a sound and change Penelope's score when destroyed
    virtual void infect(); // Infects this person
    void resetInfection(); // Uninfects this person
protected:
    // Accessors
    bool infected() const; // Returns whether this person has been infected by vomit
    
    // Actions
    virtual void doAction() = 0; // Makes this person do some action
    virtual bool paralyzed(); // Returns whether this person is paralyzed with indecision
private:
    bool m_infected; // Whether this person has been infected by vomit, always false for zombies
    int m_infection; // Number of ticks person has been infected, always 0 for zombies
    int m_score_value; // Number of points awarded/deducted when this person is destroyed
    int m_sound_infect; // Sound to be played when this person is destroyed by infection
    int m_sound_flame; // Sound to be played when this person is destroyed by flames
    bool m_paralyzed; // Whether this person is paralyzed with indecision
};

/* Penelope Class Declaration
 * - Arguably the most complex Actor class in the game!
 * - Penelope is infectable, but is never paralyzed
 * - Penelope can carry landmines, vaccines, and flamethrower charges
 * - Penelope's movement is controlled by the player in doAction()
 */
class Penelope : public Person {
public:
    // Constructor
    Penelope(int startX, int startY, StudentWorld* stWorld);
    
    // Properties
    virtual bool infectable() const; // Penelope is infectable
    
    // Accessors
    int landmines() const; // Returns the number of landmines carried by Penelope
    int flameCharges() const; // Returns the number of flamethrower charges carried by Penelope
    int vaccines() const; // Returns the number of vaccines carried by Penelope
    
    // Actions
    void adjustLandmines(int num); // Changes the number of landmines carried by Penelope by num
    void adjustFlameCharges(int num); // Changes the number of flamethrower charges carried by Penelope by num
    void adjustVaccines(int num); // Changes the number of vaccines carried by Penelope
protected:
    // Actions
    virtual void doAction(); // Detects user input and has Penelope act accordingly
    virtual bool paralyzed(); // Penelope is never paralyzed with indecision!
private:
    int m_landmines; // Number of landmines carried by Penelope
    int m_flameCharges; // Number of flamethrower charges carried by Penelope
    int m_vaccines; // Number of vaccines carried by Penelope
};

class Citizen : public Person {
public:
    // Constructor
    Citizen(int startX, int startY, StudentWorld* stWorld);
    
    // Properties
    virtual bool infectable() const; // Citizens are infectable
protected:
    // Actions
    virtual void doAction(); // Citizens follow Penelope and run from zombies
private:
    // Actions
    bool moveDirection(Direction dir); // Attempts to move in Direction dir, returns whether move was successful
};

class Zombie : public Person {
public:
    // Constructor
    Zombie(int startX, int startY, StudentWorld* stWorld, int score_value);
protected:
    // Actions
    virtual void doAction(); // Zombies have common movement behaviors
    virtual void movementPlan() = 0; // Zombies create movement plans
private:
    int m_movementPlan; // Distance to travel in current direction
};

#endif // ACTOR_H_
