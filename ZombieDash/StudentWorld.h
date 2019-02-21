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
    
    // Actor Creation Helper Functions
    void addActor(Actor* newActor); // Adds an actor to m_actors
    
    // Goodie Adjustment Helper Functions
    void adjustLandmines(int num); // Adjusts the number of landmines Penelope is carrying by num
    void adjustFlameCharges(int num); // Adjusts the number of flamethrower charges Penelope is carrying by num
    void adjustVaccines(int num); // Adjusts the number of vaccines Penelope is carrying by num
    
    // Destroy Actor Functions
    void destroyOfType(int x, int y, bool (Actor::*property)() const); // Destroys all actors overlapping (x, y) satisfying given property
    
    bool isValidDestination(int x, int y, Actor* actor) const; // Returns whether moving to the specified location is allowed
    void exitCitizens(int x, int y); // Detects which overlap with an exit and removes them
    void exitPenelope(int x, int y); // Detects and handles whether Penelope has completed the level
    bool overlapGoodie(int x, int y) const; // Returns whether Penelope overlaps with the goodie at (x, y)
    bool projectileBlocked(int x, int y); // Returns whether a projectile would be blocked by a wall or exit at (x, y)
    void infectInfectables(int x, int y); // Infects all infectables at (x, y)
    bool overlapPitDestructable(int x, int y); // Returns whether a pit-destructible actor overlaps with (x, y)
    double distPenelope(int x, int y) const; // Returns the distance to Penelope from (x, y)
    double distZombie(int x, int y) const; // Returns the distance to the nearest zombie from (x, y)
    int penelopeX() const; // Returns the x-coordinate of Penelope
    int penelopeY() const; // Returns the y-coordinate of Penelope
    bool overlapInfectable(int x, int y); // Returns whether there is an infectable that overlaps (x, y)
    Direction smartDirection(int x, int y); // Returns the direction a smart zombie should head from (x, y)
    Direction randDirection(); // Returns a random direction
private:
    std::vector<Actor*> m_actors; // Array of pointers to all actors in the game, other than Penelope
    Penelope* m_penelope; // Pointer to Penelope
    bool m_levelComplete; // Whether the level has been completed
    
    // Helper Functions
    double distance(int x1, int y1, int x2, int y2) const; // Returns the distance between (x1, y1) and (x2, y2)
    bool boundaryBoxIntersect(int x1, int y1, int x2, int y2) const; // Determines whether bounding boxes with their lower-left corner at (x1, y1) and (x2, y2) intersect
    bool overlap(int x1, int y1, int x2, int y2) const; // Determines whether actors at (x1, y1) and (x2, y2) overlap
};

#endif // STUDENTWORLD_H_
