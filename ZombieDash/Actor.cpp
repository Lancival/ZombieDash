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
void Actor::infect() {return;}

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
void Vomit::affect() {world()->infectInfectables(getX(), getY());}

// Goodie Class Implementations
Goodie::Goodie(int imageID, int startX, int startY, StudentWorld* stWorld) : Actor(imageID, startX, startY, GraphObject::right, 1, stWorld) {}
void Goodie::doSomething() {
    // If the goodie has been destroyed, do nothing
    if (!alive()) return;
    // If the goodie overlaps with Penelope, pick up the goodie
    if (world()->overlapGoodie(getX(), getY())) {
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

Person::Person(int imageID, int startX, int startY, StudentWorld* stWorld, int sound_infect, int sound_flame, int score_value) : Actor(imageID, startX, startY, GraphObject::right, 0, stWorld), m_infected(false), m_infection(0), m_sound_infect(sound_infect), m_sound_flame(sound_flame), m_score_value(score_value), m_paralyzed(false) {}
bool Person::blocksMovement() const {return true;}
bool Person::pitDestructible() const {return true;}
int Person::infection() const {return m_infection;}
bool Person::infected() const {return m_infected;}
bool Person::paralyzed() {return !(m_paralyzed = !m_paralyzed);}
void Person::infect() {m_infected = true;}
void Person::doSomething() {
    if (!alive()) return;
    if (infected() && ++m_infection >= 500) {
        destroy();
        return;
    }
    if (paralyzed()) return;
    doAction();
}
void Person::destroy() {
    setDead();
    world()->playSound(m_infection >= 500 ? m_sound_infect : m_sound_flame);
    world()->increaseScore(m_score_value);
    // If Person is a citizen create a smart/dumb zombie!!!!!!!!!!!!!!!!!
}

Penelope::Penelope(int startX, int startY, StudentWorld* stWorld) : Person(IID_PLAYER, startX, startY, stWorld, SOUND_PLAYER_DIE, SOUND_PLAYER_DIE, 0), m_landmines(0), m_flameCharges(0), m_vaccines(0) {}
bool Penelope::infectable() const {return true;}
bool Penelope::paralyzed() {return false;}
int Penelope::landmines() const {return m_landmines;}
int Penelope::flameCharges() const {return m_flameCharges;}
int Penelope::vaccines() const {return m_vaccines;}
void Penelope::adjustLandmines(int num) {m_landmines += num;}
void Penelope::adjustFlameCharges(int num) {m_flameCharges += num;}
void Penelope::adjustVaccines(int num) {m_vaccines += num;}
void Person::resetInfection() {
    m_infected = false;
    m_infection = 0;
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

Citizen::Citizen(int startX, int startY, StudentWorld* stWorld) : Person(IID_CITIZEN, startX, startY, stWorld, SOUND_ZOMBIE_BORN, SOUND_CITIZEN_DIE, -1000) {}
bool Citizen::infectable() const {return true;}
bool Citizen::moveDirection(Direction dir) {
    int x = getX();
    int y = getY();
    switch (dir) {
        case GraphObject::up:
            y += 2;
            break;
        case GraphObject::down:
            y -= 2;
            break;
        case GraphObject::left:
            x -= 2;
            break;
        case GraphObject::right:
            x += 2;
    }
    if (world()->isValidDestination(x, y, this)) {
        setDirection(dir);
        moveTo(x, y);
        return true;
    }
    return false;
}
void Citizen::doAction() {
    double dist_p = world()->distPenelope(getX(), getY()); // Distance to Penelope
    double dist_z = world()->distZombie(getX(), getY()); // Distance to the nearest zombie
    if (dist_p < dist_z && dist_p <= 80) {
        // If Penelope is closer than the nearest zombie and is closer than 80 pixels away
        int penelopeX = world()->penelopeX(); // Penelope's x-coordinate
        int penelopeY = world()->penelopeY(); // Penelope's y-coordinate
        // If zombie is on the same row or column as Penelope, try moving towards Penelope
        if (getX() == penelopeX) {
            if (getY() > penelopeY) {
                if (moveDirection(GraphObject::down)) return;
            }
            else if (moveDirection(GraphObject::up)) return;
        }
        else if (getY() == penelopeY) {
            if (getX() > penelopeX) {
                if (moveDirection(GraphObject::left)) return;
            }
            else if (moveDirection(GraphObject::right)) return;
        }
        else {
            // Otherwise, randomly try one of the two directions to move closer to Penelope, then try the other
            if (randInt(0, 1) == 0) {
                if (getX() > penelopeX) {
                    if (moveDirection(GraphObject::left)) return;
                }
                else if (moveDirection(GraphObject::right)) return;
                if (getY() > penelopeY) {
                    if (moveDirection(GraphObject::down)) return;
                }
                else if (moveDirection(GraphObject::up)) return;
            }
            else {
                if (getY() > penelopeY) {
                    if (moveDirection(GraphObject::down)) return;
                }
                else if (moveDirection(GraphObject::up)) return;
                if (getX() > penelopeX) {
                    if (moveDirection(GraphObject::left)) return;
                }
                else if (moveDirection(GraphObject::right)) return;
            }
        }
    }
    // Otherwise, try to move away from nearby zombies
    if (dist_z <= 80) {
        // Find the distances to the nearest zombie if this citizen moved for each direction
        double dist_up = world()->isValidDestination(getX(), getY()+2, this) ? world()->distZombie(getX(), getY()+2) : -1;
        double dist_down = world()->isValidDestination(getX(), getY()-2, this) ? world()->distZombie(getX(), getY()-2): -1;
        double dist_left = world()->isValidDestination(getX()-2, getY(), this) ? world()->distZombie(getX()-2, getY()) : -1;
        double dist_right = world()->isValidDestination(getX()+2, getY(), this) ? world()->distZombie(getX()+2, getY()) : -1;
        // Move (or stay still) to maximize the distance to the nearest zombie
        if (dist_up != -1 && dist_up > dist_down && dist_up > dist_left && dist_up > dist_right && dist_up > dist_z) {
            setDirection(GraphObject::up);
            moveTo(getX(), getY() + 2);
            return;
        }
        if (dist_down != -1 && dist_down > dist_left && dist_down > dist_right && dist_down > dist_z) {
            setDirection(GraphObject::down);
            moveTo(getX(), getY() - 2);
            return;
        }
        if (dist_left != -1 && dist_left > dist_right && dist_left > dist_z) {
            setDirection(GraphObject::left);
            moveTo(getX() - 2, getY());
            return;
        }
        if (dist_right != -1 && dist_right > dist_z) {
            setDirection(GraphObject::right);
            moveTo(getX() + 2, getY());
            return;
        }
    }
    // If all else fails, do nothing
}

Zombie::Zombie(int startX, int startY, StudentWorld* stWorld, int score_value) : Person(IID_ZOMBIE, startX, startY, stWorld, SOUND_NONE, SOUND_ZOMBIE_DIE, score_value), m_movementPlan(0) {}
void Zombie::doAction() {
    // If person in front, vomit on them
    if (m_movementPlan == 0) movementPlan();
    // Move 1 pixel forward
    // If blocked, set m_movementPlan to 0
}
