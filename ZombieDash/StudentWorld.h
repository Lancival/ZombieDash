#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GraphObject.h"
#include <vector>
#include <string>

class Actor;
class Penelope;

class StudentWorld : public GameWorld {
public:
    // Constructor
    StudentWorld(std::string assetPath);
    
    // Destructor
    ~StudentWorld(); // Calls cleanUp() to delete all actors
    
    // Level Functions
    virtual int     init();     // Loads information from the level data file
    virtual int     move();     // Makes each actor do something, and then deletes dead actors
    virtual void    cleanUp();  // Deletes all actors
    
    // Mutators
    void addActor(Actor* newActor); // Adds an actor to m_actors
    void destroyOfType(const int x, const int y, bool (Actor::*property)() const); // Destroys all actors overlapping (x, y) satisfying given property
    void infectInfectables(const int x, const int y); // Infects all infectables at (x, y)
    void adjustLandmines(const int num);    // Adjusts the number of landmines Penelope is carrying by num
    void adjustFlameCharges(const int num); // Adjusts the number of flamethrower charges Penelope is carrying by num
    void adjustVaccines(const int num);     // Adjusts the number of vaccines Penelope is carrying by num
    void exitCitizens(const int x, const int y); // Detects which overlap with an exit and removes them
    void exitPenelope(const int x, const int y); // Detects and handles whether Penelope has completed the level
    
    // Accessors
    bool overlapOfType(const int x, const int y, bool (Actor::*propery)() const); // Returns whether an actor satisfying the given property overlaps with (x, y)
    bool overlapGoodie(int x, int y) const; // Returns whether Penelope overlaps with a goodie at (x, y)
    bool isValidDestination(const int x, const int y, const Actor* actor) const; // Returns whether moving to the specified location is allowed
    bool projectileBlocked(const int x, const int y) const; // Returns whether a projectile would be blocked by a wall or exit at (x, y)
    double distPenelope(int x, int y) const; // Returns the distance to Penelope from (x, y)
    double distZombie(int x, int y) const; // Returns the distance to the nearest zombie from (x, y)
    int penelopeX() const; // Returns the x-coordinate of Penelope
    int penelopeY() const; // Returns the y-coordinate of Penelope
    Direction smartDirection(const int x, const int y) const; // Returns the direction a smart zombie should head from (x, y)
    Direction randDirection() const; // Returns a random direction
    
private:
    std::vector<Actor*> m_actors;   // Array of pointers to all actors in the game, other than Penelope
    Penelope* m_penelope;           // Pointer to Penelope
    bool m_levelComplete;           // Whether the level has been completed
    
    // Helper Functions
    double distance(int x1, int y1, int x2, int y2) const; // Returns the distance between (x1, y1) and (x2, y2)
    bool boundaryBoxIntersect(int x1, int y1, int x2, int y2) const; // Determines whether bounding boxes with their lower-left corner at (x1, y1) and (x2, y2) intersect
    bool overlap(int x1, int y1, int x2, int y2) const; // Determines whether actors at (x1, y1) and (x2, y2) overlap
};

#endif // STUDENTWORLD_H_
