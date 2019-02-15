#include "Actor.h"
#include "StudentWorld.h"
#include "GameConstants.h"

// Actor Class Implementations
Actor::Actor(int imageID, int startX, int startY, Direction startDirection, int depth, StudentWorld* stWorld) : GraphObject(imageID, startX, startY, startDirection, depth), m_alive(true), m_world(stWorld) {}
bool Actor::alive() const {return m_alive;}
bool Actor::flammable() const {return true;}
bool Actor::infectable() const {return false;}
bool Actor::blocksMovement() const {return false;}
bool Actor::blocksProjectiles() const {return false;}
StudentWorld* Actor::world() const {return m_world;}
void Actor::setDead() {m_alive = false;}
void Actor::destroy() {m_alive = false;}

// Terrain Class Implementations
Terrain::Terrain(int imageID, int startX, int startY, int depth, StudentWorld* stWorld) : Actor(imageID, startX, startY, GraphObject::right, depth, stWorld) {}
bool Terrain::flammable() const {return false;}

// Wall Class Implementations
Wall::Wall(int startX, int startY, StudentWorld* stWorld) : Terrain(IID_WALL, startX, startY, 0, stWorld) {}
bool Wall::blocksMovement() const {return true;}
bool Wall::blocksProjectiles() const {return true;}
void Wall::doSomething() {return;}

Exit::Exit(int startX, int startY, StudentWorld* stWorld) : Terrain(IID_EXIT, startX, startY, 1, stWorld) {}
bool Exit::blocksProjectiles() const {return true;}
void Exit::doSomething() {
    // Determine if exit overlaps with citizen
    if (false) {
        world()->increaseScore(500);
        // Set citizen's object state to dead without losing points
        world()->playSound(SOUND_CITIZEN_SAVED);
    }
    // Determine if exit overlaps with Penelope
    // If all citizens have been saved and/or died
    // Inform StudentWorld object that current level has been finished
}


// Pit Class Implementations
Pit::Pit(int startX, int startY, StudentWorld* stWorld) : Terrain(IID_PIT, startX, startY, 0, stWorld) {}
void Pit::doSomething() {
    // Kill any person or zombie that overlaps with the pit
}

// Projectile Class Implementations
Projectile::Projectile(int imageID, int startX, int startY, Direction startDirection, StudentWorld* stWorld) : Actor(imageID, startX, startY, startDirection, 0, stWorld), m_ticksLeft(2) {}
bool Projectile::flammable() const {return false;}
int Projectile::ticksLeft() const {return m_ticksLeft;}
void Projectile::doSomething() {
    // If the projectile is not alive, do nothing
    if (!alive()) return;
    
    // If the projectile was created two ticks ago, destroy it and do nothing else
    decTicks();
    if (ticksLeft() <= 0) {
        setDead();
        return;
    }
    
    // Otherwise, infect or destroy as appropriate
    affect();
}
void Projectile::decTicks() {m_ticksLeft--;}

// Flame Class Implementations
Flame::Flame(int startX, int startY, Direction startDirection, StudentWorld* stWorld) : Projectile(IID_FLAME, startX, startY, startDirection, stWorld) {}
void Flame::affect() {
    // Destroy all flammable actors this flame overlaps with
}

// Vomit Class Implementations
Vomit::Vomit(int startX, int startY, Direction startDirection, StudentWorld* stWorld) : Projectile(IID_VOMIT, startX, startY, startDirection, stWorld) {}
void Vomit::affect() {
    // Infect all infectable actors this vomit overlaps with
}

// Goodie Class Implementations
Goodie::Goodie(int imageID, int startX, int startY, StudentWorld* stWorld) : Actor(imageID, startX, startY, GraphObject::right, 1, stWorld) {}
void Goodie::doSomething() {
    if (!alive()) return;
    if (false) { // Overlaps with Penelope?
        world()->increaseScore(50);
        setDead();
        playGoodieSound();
        receiveGoodies();
    }
}
void Goodie::playGoodieSound() {world()->playSound(SOUND_GOT_GOODIE);}

// VaccineGoodie Class Implementation
VaccineGoodie::VaccineGoodie(int startX, int startY, StudentWorld* stWorld) : Goodie(IID_VACCINE_GOODIE, startX, startY, stWorld) {}
void VaccineGoodie::receiveGoodies() {
    // Inform StudentWorld object that Penelope should receive 1 dose of vaccine
}

// GasCanGoodie Class Implementation
GasCanGoodie::GasCanGoodie(int startX, int startY, StudentWorld* stWorld) : Goodie(IID_GAS_CAN_GOODIE, startX, startY, stWorld) {}
void GasCanGoodie::receiveGoodies() {
    // Inform StudentWorld object that Penelope should receive 5 flamethrower charges
}

// LandmineGoodie Class Implementation
LandmineGoodie::LandmineGoodie(int startX, int startY, StudentWorld* stWorld) : Goodie(IID_LANDMINE_GOODIE, startX, startY, stWorld) {}
void LandmineGoodie::receiveGoodies() {
    // Inform StudentWorld object that Penelope should receive 2 landmines
}

// Landmine Class Implementations
Landmine::Landmine(int startX, int startY, StudentWorld* stWorld) : Actor(IID_LANDMINE, startX, startY, GraphObject::right, 1, stWorld), m_active(false), m_safetyTicks(30) {}
int Landmine::safetyTicks() const {return m_safetyTicks;}
bool Landmine::active() const {return m_active;}
void Landmine::doSomething() {
    if (!alive()) return;
    if (!active()) {
        if (--m_safetyTicks == 0) m_active = true;
        return;
    }
    if (true) destroy(); // If landmine overlaps with citizen or Penelope or a zombie
}
void Landmine::destroy() {
    // Set state of landmine to dead
    setDead();
    
    // Play SOUND_LANDMINE_EXPLODE
    world()->playSound(SOUND_LANDMINE_EXPLODE);
    
    // Create flames at and around the landmine
    int x = getX();
    int y = getY();
    world()->createFlame(x, y);
    world()->createFlame(x+SPRITE_WIDTH, y);
    world()->createFlame(x+SPRITE_WIDTH, y+SPRITE_HEIGHT);
    world()->createFlame(x, y+SPRITE_HEIGHT);
    world()->createFlame(x-SPRITE_WIDTH, y+SPRITE_HEIGHT);
    world()->createFlame(x-SPRITE_WIDTH, y);
    world()->createFlame(x-SPRITE_WIDTH, y-SPRITE_HEIGHT);
    world()->createFlame(x, y-SPRITE_HEIGHT);
    world()->createFlame(x+SPRITE_WIDTH, y-SPRITE_HEIGHT);
    
    // Create pit object at landmine
    world()->createPit(x, y);
}
