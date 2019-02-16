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
bool Actor::pitDestructible() const {return false;}
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
    // Make any overlapping citizens exit
    world()->exitCitizens(getX(), getY());
    // If exit overlaps with Penelope and all citizens saved/dead, finish current level
    world()->exitPenelope(getX(), getY());
}

// Pit Class Implementations
Pit::Pit(int startX, int startY, StudentWorld* stWorld) : Terrain(IID_PIT, startX, startY, 0, stWorld) {}
void Pit::doSomething() {world()->destroyPitDestructables(getX(), getY());}

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
void Flame::affect() {world()->destroyFlammables(getX(), getY());}

// Vomit Class Implementations
Vomit::Vomit(int startX, int startY, Direction startDirection, StudentWorld* stWorld) : Projectile(IID_VOMIT, startX, startY, startDirection, stWorld) {}
void Vomit::affect() {
    // Infect all infectable actors this vomit overlaps with
}

// Goodie Class Implementations
Goodie::Goodie(int imageID, int startX, int startY, StudentWorld* stWorld) : Actor(imageID, startX, startY, GraphObject::right, 1, stWorld) {}
void Goodie::doSomething() {
    // If the goodie has been destroyed, do nothing
    if (!alive()) return;
    // If the goodie overlaps with Penelope, pick up the goodie
    if (world()->pickupGoodie(getX(), getY())) {
        world()->increaseScore(50);
        setDead();
        playGoodieSound();
        receiveGoodies();
    }
}
void Goodie::playGoodieSound() {world()->playSound(SOUND_GOT_GOODIE);}

// VaccineGoodie Class Implementation
VaccineGoodie::VaccineGoodie(int startX, int startY, StudentWorld* stWorld) : Goodie(IID_VACCINE_GOODIE, startX, startY, stWorld) {}
void VaccineGoodie::receiveGoodies() {world()->adjustVaccines(1);}

// GasCanGoodie Class Implementation
GasCanGoodie::GasCanGoodie(int startX, int startY, StudentWorld* stWorld) : Goodie(IID_GAS_CAN_GOODIE, startX, startY, stWorld) {}
void GasCanGoodie::receiveGoodies() {world()->adjustFlameCharges(5);}

// LandmineGoodie Class Implementation
LandmineGoodie::LandmineGoodie(int startX, int startY, StudentWorld* stWorld) : Goodie(IID_LANDMINE_GOODIE, startX, startY, stWorld) {}
void LandmineGoodie::receiveGoodies() {world()->adjustLandmines(2);}

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
    if (world()->overlapPitDestructable(getX(), getY())) destroy();
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
    if (!world()->projectileBlocked(x+SPRITE_WIDTH, y))
        world()->createFlame(x+SPRITE_WIDTH, y);
    if (!world()->projectileBlocked(x+SPRITE_WIDTH, y+SPRITE_HEIGHT))
        world()->createFlame(x+SPRITE_WIDTH, y+SPRITE_HEIGHT);
    if (!world()->projectileBlocked(x, y+SPRITE_HEIGHT))
        world()->createFlame(x, y+SPRITE_HEIGHT);
    if (!world()->projectileBlocked(x-SPRITE_WIDTH, y+SPRITE_HEIGHT))
        world()->createFlame(x-SPRITE_WIDTH, y+SPRITE_HEIGHT);
    if (!world()->projectileBlocked(x-SPRITE_WIDTH, y))
        world()->createFlame(x-SPRITE_WIDTH, y);
    if (!world()->projectileBlocked(x-SPRITE_WIDTH, y-SPRITE_HEIGHT))
        world()->createFlame(x-SPRITE_WIDTH, y-SPRITE_HEIGHT);
    if (!world()->projectileBlocked(x, y-SPRITE_HEIGHT))
        world()->createFlame(x, y-SPRITE_HEIGHT);
    if (!world()->projectileBlocked(x+SPRITE_WIDTH, y-SPRITE_HEIGHT))
        world()->createFlame(x+SPRITE_WIDTH, y-SPRITE_HEIGHT);
    // Create pit object at landmine
    world()->createPit(x, y);
}

Person::Person(int imageID, int startX, int startY, StudentWorld* stWorld) : Actor(imageID, startX, startY, GraphObject::right, 0, stWorld), m_infected(false), m_infection(0) {}
bool Person::infectable() const {return true;}
bool Person::blocksMovement() const {return true;}
bool Person::pitDestructible() const {return true;}
int Person::infection() const {return m_infection;}
bool Person::infected() const {return m_infected;}
void Person::doSomething() {
    if (!alive()) return;
    if (infected() && ++m_infection >= 500) {
        destroy();
        return;
    }
    doAction();
}
void Person::resetInfection() {
    m_infected = false;
    m_infection = 0;
}

Penelope::Penelope(int startX, int startY, StudentWorld* stWorld) : Person(IID_PLAYER, startX, startY, stWorld), m_landmines(0), m_flameCharges(0), m_vaccines(0) {}
int Penelope::landmines() const {return m_landmines;}
int Penelope::flameCharges() const {return m_flameCharges;}
int Penelope::vaccines() const {return m_vaccines;}
void Penelope::adjustLandmines(int num) {m_landmines += num;}
void Penelope::adjustFlameCharges(int num) {m_flameCharges += num;}
void Penelope::adjustVaccines(int num) {m_vaccines += num;}
void Penelope::destroy() {
    setDead();
    world()->playSound(SOUND_PLAYER_DIE);
}
void Penelope::doAction() {
    int key;
    if (world()->getKey(key)) {
        switch (key) {
            case KEY_PRESS_LEFT:
                setDirection(GraphObject::left);
                if (world()->isValidDestination(getX()-4, getY(), this))
                    moveTo(getX()-4, getY());
                break;
            case KEY_PRESS_RIGHT:
                setDirection(GraphObject::right);
                if (world()->isValidDestination(getX()+4, getY(), this))
                    moveTo(getX()+4, getY());
                break;
            case KEY_PRESS_UP:
                setDirection(GraphObject::up);
                if (world()->isValidDestination(getX(), getY()+4, this))
                    moveTo(getX(), getY()+4);
                break;
            case KEY_PRESS_DOWN:
                setDirection(GraphObject::down);
                if (world()->isValidDestination(getX(), getY()-4, this))
                    moveTo(getX(), getY()-4);
                break;
            case KEY_PRESS_SPACE:
                if (m_flameCharges > 0) {
                    m_flameCharges--;
                    world()->playSound(SOUND_PLAYER_FIRE);
                    switch (getDirection()) {
                        case GraphObject::right:
                            for (int i = 1; i <= 3; i++) {
                                if (world()->projectileBlocked(getX() + SPRITE_WIDTH*i, getY())) return;
                                world()->createFlame(getX()+SPRITE_WIDTH*i, getY(), GraphObject::right);
                            }
                            break;
                        case GraphObject::left:
                            for (int i = 1; i <= 3; i++) {
                                if (world()->projectileBlocked(getX() - SPRITE_WIDTH*i, getY())) return;
                                world()->createFlame(getX()-SPRITE_WIDTH*i, getY(), GraphObject::left);
                            }
                            break;
                        case GraphObject::up:
                            for (int i = 1; i <= 3; i++) {
                                if (world()->projectileBlocked(getX(), getY() + SPRITE_HEIGHT*i)) return;
                                world()->createFlame(getX(), getY()+SPRITE_HEIGHT*i, GraphObject::up);
                            }
                            break;
                        case GraphObject::down:
                            for (int i = 1; i <= 3; i++) {
                                if (world()->projectileBlocked(getX(), getY() - SPRITE_HEIGHT*i)) return;
                                world()->createFlame(getX(), getY()-SPRITE_HEIGHT*i, GraphObject::down);
                            }
                    }
                }
                break;
            case KEY_PRESS_TAB:
                if (m_landmines > 0) {
                    m_landmines--;
                    world()->createLandmine(getX(), getY());
                }
                break;
            case KEY_PRESS_ENTER:
                if (m_vaccines > 0) {
                    m_vaccines--;
                    resetInfection();
                }
        }
    }
}
