#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include <vector>
#include <string>

class Actor;
class Penelope;

class StudentWorld : public GameWorld {
public:
    // Constructor
    StudentWorld(std::string assetPath);
    
    // Destructor
    ~StudentWorld(); // Deletes all actors
    
    // Level Functions
    virtual int init(); // Loads information from the level data file
    virtual int move(); // Makes each actor do something, and then deletes dead actors
    virtual void cleanUp(); // Deletes all actors
    
    // Helper Functions
    void createFlame(int startX, int startY); // Creates a flame at the specified location
    void createPit(int startX, int startY); // Creates a pit at the specified location
private:
    std::vector<Actor*> m_actors; // Array of pointers to all actors in the game, other than Penelope
    Penelope* m_penelope; // Pointer to Penelope
};

#endif // STUDENTWORLD_H_
