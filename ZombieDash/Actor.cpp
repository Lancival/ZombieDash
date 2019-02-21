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
void Pit::doSomething() {world()->destroyOfType(getX(), getY(), &Actor::pitDestructible);}

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
void Flame::affect() {world()->destroyOfType(getX(), getY(), &Actor::flammable);}

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
    if (world()->overlapOfType(getX(), getY(), &Actor::pitDestructible))
        destroy();
}
void Landmine::destroy() {
    // Set state of landmine to dead
    setDead();
    // Play SOUND_LANDMINE_EXPLODE
    world()->playSound(SOUND_LANDMINE_EXPLODE);
    // Create flames at and around the landmine
    int x = getX();
    int y = getY();
    world()->addActor(new Flame(x, y, GraphObject::right, world()));
    if (!world()->projectileBlocked(x+SPRITE_WIDTH, y))
        world()->addActor(new Flame(x+SPRITE_WIDTH, y, GraphObject::right, world()));
    if (!world()->projectileBlocked(x+SPRITE_WIDTH, y+SPRITE_HEIGHT))
        world()->addActor(new Flame(x+SPRITE_WIDTH, y+SPRITE_HEIGHT, GraphObject::right, world()));
    if (!world()->projectileBlocked(x, y+SPRITE_HEIGHT))
        world()->addActor(new Flame(x, y+SPRITE_HEIGHT, GraphObject::right, world()));
    if (!world()->projectileBlocked(x-SPRITE_WIDTH, y+SPRITE_HEIGHT))
        world()->addActor(new Flame(x-SPRITE_WIDTH, y+SPRITE_HEIGHT, GraphObject::right, world()));
    if (!world()->projectileBlocked(x-SPRITE_WIDTH, y))
        world()->addActor(new Flame(x-SPRITE_WIDTH, y, GraphObject::right, world()));
    if (!world()->projectileBlocked(x-SPRITE_WIDTH, y-SPRITE_HEIGHT))
        world()->addActor(new Flame(x-SPRITE_WIDTH, y-SPRITE_HEIGHT, GraphObject::right, world()));
    if (!world()->projectileBlocked(x, y-SPRITE_HEIGHT))
        world()->addActor(new Flame(x, y-SPRITE_HEIGHT, GraphObject::right, world()));
    if (!world()->projectileBlocked(x+SPRITE_WIDTH, y-SPRITE_HEIGHT))
        world()->addActor(new Flame(x+SPRITE_WIDTH, y-SPRITE_HEIGHT, GraphObject::right, world()));
    // Create pit object at landmine
    world()->addActor(new Pit(x, y, world()));
}

Person::Person(int imageID, int startX, int startY, StudentWorld* stWorld, int sound_infect, int sound_flame, int score_value, int step_distance) : Actor(imageID, startX, startY, GraphObject::right, 0, stWorld), m_infected(false), m_infection(0), m_sound_infect(sound_infect), m_sound_flame(sound_flame), m_score_value(score_value), m_paralyzed(false), m_step_distance(step_distance) {}
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
    if (m_infection >= 500 && world()->distPenelope(getX(), getY()) != 0) {
        if (randInt(1, 10) <= 3)
            world()->addActor(new Zombie(getX(), getY(), world()));
        else
            world()->addActor(new SmartZombie(getX(), getY(), world()));
    }
    if (!infectable() && m_score_value == 1000 && randInt(1, 10) == 1)
        world()->addActor(new VaccineGoodie(getX(), getY(), world()));
}
bool Person::moveDirection(Direction dir) {
    int x = getX();
    int y = getY();
    switch (dir) {
        case GraphObject::up:
            y += m_step_distance;
            break;
        case GraphObject::down:
            y -= m_step_distance;
            break;
        case GraphObject::left:
            x -= m_step_distance;
            break;
        case GraphObject::right:
            x += m_step_distance;
    }
    if (world()->isValidDestination(x, y, this)) {
        setDirection(dir);
        moveTo(x, y);
        return true;
    }
    return false;
}

Penelope::Penelope(int startX, int startY, StudentWorld* stWorld) : Person(IID_PLAYER, startX, startY, stWorld, SOUND_PLAYER_DIE, SOUND_PLAYER_DIE, 0, 4), m_landmines(0), m_flameCharges(0), m_vaccines(0) {}
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
                moveDirection(GraphObject::left);
                break;
            case KEY_PRESS_RIGHT:
                setDirection(GraphObject::right);
                moveDirection(GraphObject::right);
                break;
            case KEY_PRESS_UP:
                setDirection(GraphObject::up);
                moveDirection(GraphObject::up);
                break;
            case KEY_PRESS_DOWN:
                setDirection(GraphObject::down);
                moveDirection(GraphObject::down);
                break;
            case KEY_PRESS_SPACE:
                if (m_flameCharges > 0) {
                    m_flameCharges--;
                    world()->playSound(SOUND_PLAYER_FIRE);
                    switch (getDirection()) {
                        case GraphObject::right:
                            for (int i = 1; i <= 3; i++) {
                                if (world()->projectileBlocked(getX() + SPRITE_WIDTH*i, getY())) return;
                                world()->addActor(new Flame(getX()+SPRITE_WIDTH*i, getY(), GraphObject::right, world()));
                            }
                            break;
                        case GraphObject::left:
                            for (int i = 1; i <= 3; i++) {
                                if (world()->projectileBlocked(getX() - SPRITE_WIDTH*i, getY())) return;
                                world()->addActor(new Flame(getX()-SPRITE_WIDTH*i, getY(), GraphObject::right, world()));
                            }
                            break;
                        case GraphObject::up:
                            for (int i = 1; i <= 3; i++) {
                                if (world()->projectileBlocked(getX(), getY() + SPRITE_HEIGHT*i)) return;
                                world()->addActor(new Flame(getX(), getY()+SPRITE_HEIGHT*i, GraphObject::up, world()));
                            }
                            break;
                        case GraphObject::down:
                            for (int i = 1; i <= 3; i++) {
                                if (world()->projectileBlocked(getX(), getY() - SPRITE_HEIGHT*i)) return;
                                world()->addActor(new Flame(getX(), getY()-SPRITE_HEIGHT*i, GraphObject::down, world()));
                            }
                    }
                }
                break;
            case KEY_PRESS_TAB:
                if (m_landmines > 0) {
                    m_landmines--;
                    world()->addActor(new Landmine(getX(), getY(), world()));
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

Citizen::Citizen(int startX, int startY, StudentWorld* stWorld) : Person(IID_CITIZEN, startX, startY, stWorld, SOUND_ZOMBIE_BORN, SOUND_CITIZEN_DIE, -1000, 2) {}
bool Citizen::infectable() const {return true;}
void Citizen::infect() {
    Person::infect();
    world()->playSound(SOUND_CITIZEN_INFECTED);
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

Zombie::Zombie(int startX, int startY, StudentWorld* stWorld, int score_value) : Person(IID_ZOMBIE, startX, startY, stWorld, SOUND_NONE, SOUND_ZOMBIE_DIE, score_value, 1), m_movementPlan(0) {}
void Zombie::doAction() {
    // Vomit on nearby infectables
    if (vomit()) return;
    if (m_movementPlan == 0) {
        m_movementPlan = randInt(3, 10);
        movementPlan();
    }
    // Move 1 pixel forward, and decrement movement plan distance. If blocked, set movement plan to 0 instead.
    m_movementPlan = moveDirection(getDirection()) ? m_movementPlan - 1 : 0;
}
bool Zombie::vomit() {
    int vomitX = getX();
    int vomitY = getY();
    switch (getDirection()) {
        case GraphObject::right:
            vomitX += SPRITE_WIDTH;
            break;
        case GraphObject::left:
            vomitX -= SPRITE_WIDTH;
            break;
        case GraphObject::up:
            vomitY += SPRITE_HEIGHT;
            break;
        case GraphObject::down:
            vomitY -= SPRITE_HEIGHT;
    }
    if (world()->overlapOfType(vomitX, vomitY, &Actor::infectable) && randInt(1,3) == 1) {
        world()->addActor(new Vomit(vomitX, vomitY, getDirection(), world()));
        world()->playSound(SOUND_ZOMBIE_VOMIT);
        return true;
    }
    return false;
}
void Zombie::movementPlan() {setDirection(world()->randDirection());}

SmartZombie::SmartZombie(int startX, int startY, StudentWorld* stWorld) : Zombie(startX, startY, stWorld, 2000) {}
void SmartZombie::movementPlan() {setDirection(world()->smartDirection(getX(), getY()));}
