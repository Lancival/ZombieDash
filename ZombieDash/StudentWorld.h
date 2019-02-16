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
    virtual int init(); // Loads information from the level data file
    virtual int move(); // Makes each actor do something, and then deletes dead actors
    virtual void cleanUp(); // Deletes all actors
    
    // Helper Functions
    void createFlame(int startX, int startY, Direction dir = GraphObject::right); // Creates a flame at the specified location
    void createPit(int startX, int startY); // Creates a pit at the specified location
    void createLandmine(int startX, int startY); // Creates a landmine at the specified location
    void adjustLandmines(int num); // Adjusts the number of landmines Penelope is carrying by num
    void adjustFlameCharges(int num); // Adjusts the number of flamethrower charges Penelope is carrying by num
    void adjustVaccines(int num); // Adjusts the number of vaccines Penelope is carrying by num
    bool isValidDestination(int x, int y, Actor* actor); // Returns whether moving to the specified location is allowed
    void exitCitizens(int x, int y); // Detects which overlap with an exit and removes them
    void exitPenelope(int x, int y); // Detects and handles whether Penelope has completed the level
    bool pickupGoodie(int x, int y); // Returns whether Penelope overlaps with the goodie at (x, y)
    bool projectileBlocked(int x, int y); // Returns whether a projectile would be blocked by a wall or exit at (x, y)
    void destroyFlammables(int x, int y); // Destroys all flammables at (x, y)
    void destroyPitDestructables(int x, int y); // Destroys all pit-destructible actors at (x, y)
    bool overlapPitDestructable(int x, int y); // Returns whether a pit-destructible actor overlaps with (x, y)
private:
    std::vector<Actor*> m_actors; // Array of pointers to all actors in the game, other than Penelope
    Penelope* m_penelope; // Pointer to Penelope
    bool m_levelComplete; // Whether the level has been completed
    
    // Helper Functions
    bool boundaryBoxIntersect(int x1, int y1, int x2, int y2); // Determines whether bounding boxes with their lower-left corner at (x1, y1) and (x2, y2) intersect
    bool overlap(int x1, int y1, int x2, int y2); // Determines whether actors at (x1, y1) and (x2, y2) overlap
};

#endif // STUDENTWORLD_H_
