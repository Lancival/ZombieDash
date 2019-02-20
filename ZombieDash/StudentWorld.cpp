#include "StudentWorld.h"
#include "GameConstants.h"
#include "Level.h"
#include "Actor.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cmath>
using namespace std;

GameWorld* createStudentWorld(string assetPath) {return new StudentWorld(assetPath);}

StudentWorld::StudentWorld(string assetPath) : GameWorld(assetPath), m_actors(), m_penelope(NULL), m_levelComplete(false) {}

StudentWorld::~StudentWorld() {
    cleanUp();
}

int StudentWorld::init() {
    Level level(assetPath());
    
    // Create string containing name of file to be loaded
    ostringstream levelFileStream;
    levelFileStream << "level";
    levelFileStream.fill('0');
    levelFileStream << setw(2) << getLevel() << ".txt";
    string levelFile = levelFileStream.str();
    
    // Load the file
    Level::LoadResult result = level.loadLevel(levelFile);
    if (result == Level::load_fail_file_not_found) {
        cerr << "Error: Could not find " << levelFile << " data file." << endl;
        return GWSTATUS_LEVEL_ERROR; // Should this return something else when file cannot be found?
    }
    else if (result == Level::load_fail_bad_format) {
        cerr << "Error: " << levelFile << " data file improperly formatted." << endl;
        return GWSTATUS_LEVEL_ERROR;
    }
    else {
        cerr << "Successfully loaded level " << to_string(getLevel()) << "!" << endl;
        Level::MazeEntry ge;
        for (int x = 0; x < LEVEL_WIDTH; x++) {
            for (int y = 0; y < LEVEL_HEIGHT; y++) {
                ge = level.getContentsOf(x, y);
                switch (ge) {
                    case Level::empty:
                        break;
                        
                    // Finish writing these
                    case Level::smart_zombie:
                        break;
                        
                    case Level::dumb_zombie:
                        m_actors.push_back(new Zombie(x*SPRITE_HEIGHT, y*SPRITE_WIDTH, this, 1000));
                        break;
                    case Level::player:
                        m_penelope = new Penelope(x*SPRITE_WIDTH, y*SPRITE_HEIGHT, this);
                        break;
                    case Level::citizen:
                        m_actors.push_back(new Citizen(x*SPRITE_WIDTH, y*SPRITE_HEIGHT, this));
                        break;
                    case Level::exit:
                        m_actors.push_back(new Exit(x*SPRITE_WIDTH, y*SPRITE_HEIGHT, this));
                        break;
                    case Level::wall:
                        m_actors.push_back(new Wall(x*SPRITE_WIDTH, y*SPRITE_HEIGHT, this));
                        break;
                    case Level::pit:
                        m_actors.push_back(new Pit(x*SPRITE_WIDTH, y*SPRITE_HEIGHT, this));
                        break;
                    case Level::vaccine_goodie:
                        m_actors.push_back(new VaccineGoodie(x*SPRITE_WIDTH, y*SPRITE_HEIGHT, this));
                        break;
                    case Level::gas_can_goodie:
                        m_actors.push_back(new GasCanGoodie(x*SPRITE_WIDTH, y*SPRITE_HEIGHT, this));
                        break;
                    case Level::landmine_goodie:
                        m_actors.push_back(new LandmineGoodie(x*SPRITE_WIDTH, y*SPRITE_HEIGHT, this));
                        break;
                }
            }
        }
    }
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move() {
    // Tell each actor to do something
    m_penelope->doSomething();
    for (int i = 0; i < m_actors.size(); i++) {
        if (m_actors[i]->alive())
            m_actors[i]->doSomething();
        // Check if Penelope is still alive
        if (!m_penelope->alive()) {
            decLives();
            return GWSTATUS_PLAYER_DIED;
        }
        // Check if Penelope has completed the level
        if (m_levelComplete) return GWSTATUS_FINISHED_LEVEL;
    }
    
    // Delete all dead actors
    for (int i = 0; i < m_actors.size(); i++) {
        if (!m_actors[i]->alive()) {
            delete m_actors[i];
            m_actors.erase(m_actors.begin() + i--);
        }
    }
    
    // Update the status text
    ostringstream statusTextStream;
    statusTextStream.fill('0');
    if (getScore() < 0)
        statusTextStream << "Score: -" << setw(5) << abs(getScore());
    else
        statusTextStream << "Score: " << setw(6) << getScore();
    statusTextStream << "  Level: " << getLevel();
    statusTextStream << "  Lives: " << getLives();
    statusTextStream << "  Vaccines: " << m_penelope->vaccines();
    statusTextStream << "  Flames: " << m_penelope->flameCharges();
    statusTextStream << "  Mines: " << m_penelope->landmines();
    statusTextStream << "  Infected: " << m_penelope->infection();
    string statusText = statusTextStream.str();
    setGameStatText(statusText);
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp() {
    while (m_actors.size() > 0) {
        delete m_actors[0];
        m_actors.erase(m_actors.begin());
    }
    delete m_penelope;
    m_penelope = NULL;
    m_levelComplete = false;
}

void StudentWorld::createFlame(int startX, int startY, Direction dir) {m_actors.push_back(new Flame(startX, startY, dir, this));}
void StudentWorld::createPit(int startX, int startY) {m_actors.push_back(new Pit(startX, startY, this));}
void StudentWorld::createLandmine(int startX, int startY) {m_actors.push_back(new Landmine(startX, startY, this));}

void StudentWorld::adjustLandmines(int num) {m_penelope->adjustLandmines(num);}
void StudentWorld::adjustFlameCharges(int num) {m_penelope->adjustFlameCharges(num);}
void StudentWorld::adjustVaccines(int num) {m_penelope->adjustVaccines(num);}

bool StudentWorld::boundaryBoxIntersect(int x1, int y1, int x2, int y2) const {
    if (abs(x1-x2) < SPRITE_WIDTH-1 && abs(y1-y2) < SPRITE_HEIGHT-1)
        return true;
    return false;
}

bool StudentWorld::isValidDestination(int x, int y, Actor* actor) const {
    if (actor != m_penelope)
        if (boundaryBoxIntersect(x, y, m_penelope->getX(), m_penelope->getY()))
            return false;
    for (int i = 0; i < m_actors.size(); i++)
        if (m_actors[i]->blocksMovement() && m_actors[i]->alive() && m_actors[i] != actor)
            if (boundaryBoxIntersect(x, y, m_actors[i]->getX(), m_actors[i]->getY()))
                return false;
    return true;
}

double StudentWorld::distance(int x1, int y1, int x2, int y2) const {
    return pow(pow(x1-x2,2) + pow(y1-y2, 2), 0.5);
}

bool StudentWorld::overlap(int x1, int y1, int x2, int y2) const {
    return (distance(x1, y1, x2, y2) <= 10) ? true : false;
}

void StudentWorld::exitCitizens(int x, int y) {
    for (int i = 0; i < m_actors.size(); i++) {
        if (m_actors[i]->alive() && m_actors[i]->infectable()) {
            if (overlap(x, y, m_actors[i]->getX(), m_actors[i]->getY())) {
                increaseScore(500);
                m_actors[i]->setDead();
                playSound(SOUND_CITIZEN_SAVED);
            }
        }
    }
}

void StudentWorld::exitPenelope(int x, int y) {
    // Check if Penelope overlaps with the exit
    if (!overlap(x, y, m_penelope->getX(), m_penelope->getY())) return;
    // Check if there are any remaining live citizens
    for (int i = 0; i < m_actors.size(); i++)
        if (m_actors[i]->alive() && m_actors[i]->infectable()) return;
    playSound(SOUND_LEVEL_FINISHED);
    m_levelComplete = true;
}

bool StudentWorld::overlapGoodie(int x, int y) const {
    return overlap(x, y, m_penelope->getX(), m_penelope->getY());
}

bool StudentWorld::projectileBlocked(int x, int y) {
    for (int i = 0; i < m_actors.size(); i++)
        if (m_actors[i]->alive() && m_actors[i]->blocksProjectiles())
            if (overlap(x, y, m_actors[i]->getX(), m_actors[i]->getY()))
                return true;
    return false;
}

void StudentWorld::destroyFlammables(int x, int y) {
    for (int i = 0; i < m_actors.size(); i++)
        if (m_actors[i]->alive() && m_actors[i]->flammable())
            if (overlap(x, y, m_actors[i]->getX(), m_actors[i]->getY()))
                m_actors[i]->destroy();
    if (m_penelope->alive() && overlap(x, y, m_penelope->getX(), m_penelope->getY()))
        m_penelope->destroy();
}

void StudentWorld::infectInfectables(int x, int y) {
    for (int i = 0; i < m_actors.size(); i++)
        if (m_actors[i]->alive() && m_actors[i]->infectable())
            if (overlap(x, y, m_actors[i]->getX(), m_actors[i]->getY()))
                m_actors[i]->infect();
    if (m_penelope->alive() && overlap(x, y, m_penelope->getX(), m_penelope->getY()))
        m_penelope->infect();
}

void StudentWorld::destroyPitDestructables(int x, int y) {
    for (int i = 0; i < m_actors.size(); i++)
        if (m_actors[i]->alive() && m_actors[i]->pitDestructible())
            if (overlap(x, y, m_actors[i]->getX(), m_actors[i]->getY()))
                m_actors[i]->destroy();
    if (m_penelope->alive() && overlap(x, y, m_penelope->getX(), m_penelope->getY()))
        m_penelope->destroy();
}

bool StudentWorld::overlapPitDestructable(int x, int y) {
    if (m_penelope->alive() && overlap(x, y, m_penelope->getX(), m_penelope->getY()))
        return true;
    for (int i = 0; i < m_actors.size(); i++)
        if (m_actors[i]->alive() && m_actors[i]->pitDestructible())
            if (overlap(x, y, m_actors[i]->getX(), m_actors[i]->getY()))
                return true;
    return false;
}

double StudentWorld::distPenelope(int x, int y) const {return distance(x, y, m_penelope->getX(), m_penelope->getY());}
double StudentWorld::distZombie(int x, int y) const {
    int min = INT_MAX;
    for (int i = 0; i < m_actors.size(); i++) {
        if (m_actors[i]->alive() && m_actors[i]->pitDestructible() && !m_actors[i]->infectable()) {
            double dist = distance(x, y, m_actors[i]->getX(), m_actors[i]->getY());
            if (dist < min) min = dist;
        }
    }
    return min;
}

int StudentWorld::penelopeX() const {return m_penelope->getX();}
int StudentWorld::penelopeY() const {return m_penelope->getY();}

void StudentWorld::createVomit(int startX, int startY, Direction dir) {m_actors.push_back(new Vomit(startX, startY, dir, this));}
bool StudentWorld::overlapInfectable(int x, int y) {
    if (m_penelope->alive() && overlap(x, y, m_penelope->getX(), m_penelope->getY()))
        return true;
    for (int i = 0; i < m_actors.size(); i++)
        if (m_actors[i]->alive() && m_actors[i]->infectable())
            if (overlap(x, y, m_actors[i]->getX(), m_actors[i]->getY()))
                return true;
    return false;
}

void StudentWorld::createZombie(int startX, int startY) {
    if (randInt(1, 10) <= 3)
        m_actors.push_back(new Zombie(startX, startY, this, 1000));
    else
        m_actors.push_back(new Zombie(startX, startY, this, 2000));
    // Change when smart zombies implemented
}
