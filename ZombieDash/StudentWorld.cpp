#include "StudentWorld.h"
#include "GameConstants.h"
#include "Level.h"
#include "Actor.h"
#include <iostream>
#include <sstream>
#include <iomanip>
using namespace std;

GameWorld* createStudentWorld(string assetPath) {return new StudentWorld(assetPath);}

StudentWorld::StudentWorld(string assetPath) : GameWorld(assetPath), m_actors(), m_penelope(NULL) {}

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
    if (result == Level::load_fail_file_not_found)
        cerr << "Error: Could not find level" << to_string(getLevel()) << ".txt data file." << endl;
    else if (result == Level::load_fail_bad_format)
        cerr << "Error: level" << to_string(getLevel()) << ".txt data file improperly formatted." << endl;
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
                        break;
                    case Level::player:
                        break;
                    case Level::citizen:
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
    for (int i = 0; i < m_actors.size(); i++) {
        if (m_actors[i]->alive())
            m_actors[i]->doSomething();
        // If Penelope dies, immediately return GWSTATUS_PLAYER_DIED
        // If Penelope and all citizens have escaped, immediately return GWSTATUS_FINISHED_LEVEL
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
    statusTextStream << "Score: " << setw(6) << getScore();
    statusTextStream << "  Level: " << getLevel();
    statusTextStream << "  Lives: " << getLives();
    statusTextStream << "  Vaccines: " << "0"; // Finish this line
    statusTextStream << "  Flames: " << "0"; // Finish this line
    statusTextStream << "  Mines: " << "0"; // Finish this line
    statusTextStream << "  Infected: " << "0"; // Finish this line
    string statusText = statusTextStream.str();
    setGameStatText(statusText);
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp() {for (int i = 0; i < m_actors.size(); i++) delete m_actors[i];}

void StudentWorld::createFlame(int startX, int startY) {m_actors.push_back(new Flame(startX, startY, GraphObject::right, this));}
void StudentWorld::createPit(int startX, int startY) {m_actors.push_back(new Pit(startX, startY, this));}
