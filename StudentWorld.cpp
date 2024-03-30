#include "StudentWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include <string>
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
    return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h, and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath), m_avatar(nullptr), bonus(1000), crystalCount(0), levelCompleted(false), crystalFlag(false), m_currentLevel(0)
{
}

int StudentWorld::init()
{
    Level lev(assetPath());
    ostringstream oss;
    oss << "level" << setw(2) << setfill('0') << getLevel(); //loads level properly with 2 digits and filled with 0s
    string curlLevel = oss.str() + ".txt";
    Level::LoadResult result = lev.loadLevel(curlLevel);
    if (result == Level:: load_fail_bad_format) {
        return GWSTATUS_LEVEL_ERROR;
    }
    else if (result == Level::load_fail_file_not_found) {
        return GWSTATUS_PLAYER_WON;
    }
    else if (result == Level::load_success){
        for (int x = 0; x < VIEW_WIDTH; ++x) {
            for (int y = 0; y < VIEW_HEIGHT; ++y) {
                Level::MazeEntry item = lev.getContentsOf(x, y);
                switch (item){
                    case Level::empty:
                        break;
                    case Level::player:
                        m_avatar = new Avatar(x, y, this);
                        m_actors.push_back(m_avatar);
                        break;
                    case Level::exit:
                        m_actors.push_back(new Exit(x, y, this));
                        break;
                    case Level::horiz_ragebot:
                        m_actors.push_back(new Ragebot(x, y, this, 0));
                        break;
                    case Level::vert_ragebot:
                        m_actors.push_back(new Ragebot(x, y, this, 270));
                        break;
                    case Level::thiefbot_factory:
                        m_actors.push_back(new ThiefBotFactory(x,y,this, 0));
                        break;
                    case Level::mean_thiefbot_factory:
                        m_actors.push_back(new ThiefBotFactory(x,y,this, 1));
                        break;
                    case Level::wall:
                        m_actors.push_back(new Wall(x, y, this));
                        break;
                    case Level::marble:
                        m_actors.push_back(new Marble(x,y,this));
                        break;
                    case Level::pit:
                        m_actors.push_back(new Pit(x,y, this));
                        break;
                    case Level::crystal:
                        m_actors.push_back(new Crystal(x,y,this));
                        break;
                    case Level::restore_health:
                        m_actors.push_back(new RestoreHealthGoodie(x,y,this, 500));
                        break;
                    case Level::extra_life:
                        m_actors.push_back(new ExtraLifeGoodie(x,y,this, 1000));
                        break;
                    case Level::ammo:
                        m_actors.push_back(new AmmoGoodie(x,y,this, 100));
                        break;
                        
                }
            }
        }
        return GWSTATUS_CONTINUE_GAME;
    }
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    // This code is here merely to allow the game to build, run, and terminate after you type q
    setGameStatText("Game will end when you type q");
    updateDisplayText();
    list<Actor*>:: iterator it;
    for (it = m_actors.begin(); it != m_actors.end();) { //loops throught list of actors
        Actor* actor = *it;
        if (actor->isActive()) {
            actor->doSomething();
            if(!m_avatar->isActive()) {
                return GWSTATUS_PLAYER_DIED;
            }
            if (levelCompleted) {
                playSound(SOUND_FINISHED_LEVEL);// do the process for whne the level is finished
                increaseScore(2000);
                increaseScore(bonus);
                m_currentLevel++;
                if (getLevel() == 100) {
                    return GWSTATUS_PLAYER_WON;
                }
                return GWSTATUS_FINISHED_LEVEL;
            }
            ++it;
        }
        else {
            delete *it;
            it = m_actors.erase(it);
        }
    }
    if (bonus > 0) { //this is what keeps track of the bonus from the score going down
        bonus--;
    }
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    list<Actor*>:: iterator it;
    it = m_actors.begin();
    while(it != m_actors.end()) {
        delete *it;
        it = m_actors.erase(it);
    }
    m_avatar = nullptr; //reset things in cleanup for new level
    levelCompleted = false;
    crystalFlag = false;
    crystalCount = 0;
    bonus = 1000;
}
StudentWorld::~StudentWorld() {
    cleanUp();
}
void StudentWorld::updateDisplayText() {
    ostringstream oss;
    oss << "  Score: " << setw(7) << setfill('0') << getScore();
    oss << "  Level: "  << setw(2) << setfill('0') << getLevel();
    oss << "  Lives: " << setw(2) << setfill(' ') << getLives();
    oss << "  Health: " << setw(3) << setfill(' ') << m_avatar->getHp()/20.0*100 << "%"; //followed spec
    oss << "  Ammo: "  << setw(3) << setfill(' ') << m_avatar->getPeas();
    oss << "  Bonus: " << setw(4) << setfill(' ') << bonus;
    string s = oss.str();
    setGameStatText(s);
}

bool StudentWorld::validWalk(int x, int y) const {
    list<Actor*>:: const_iterator it; //made it a const iterator bc const function
    it = m_actors.begin();
    while( it != m_actors.end()) {
        if ((*it)->getX() == x && (*it)->getY() == y) {
            if ((*it)->goThroughActor()) { //if its like a goodie or smth where the player can go throuhg it doesnt matter
                it++;
                continue;
            }
            else if ((*it)->isPushable()) {
                bool successfulMove = false;
                int dir = m_avatar->getDirection();
                switch (dir) {
                    case 0: //right
                        if (canMarbleMoveTo(x+1, y)) { //checks if marble can move
                            (*it)->moveTo(x+1, y); //essnetially im moving the marble then moving the player
                            successfulMove = true;
                        }
                        break;
                    case 270: //down
                        if (canMarbleMoveTo(x, y-1)) {
                            (*it)->moveTo(x, y-1);
                            successfulMove = true;
                        }
                        break;
                    case 180: //left
                        if (canMarbleMoveTo(x-1, y)) {
                            (*it)->moveTo(x-1, y);
                            successfulMove = true;
                        }
                        break;
                    case 90: //up
                        if (canMarbleMoveTo(x, y+1)) {
                            (*it)->moveTo(x, y+1);
                            successfulMove = true;
                        }
                        break;
                }
                return successfulMove;
            }
            else {
                return false;
            }
        }
        it++;
    }
    return true;
}

bool StudentWorld::canMarbleMoveTo(int x, int y) const {
    list<Actor*>::const_iterator it;
    it = m_actors.begin();
    while (it != m_actors.end()) {
        if ((*it)->getX() == x && (*it)->getY() == y && !(*it)->allowMarble()) { //check if its not a valid walk as in x and y r
            return false;
        }
        it++;
    }
    return true; // filters the rest of the cases 1. if none x and y are equal move 2. if its equal but u can move in .
}


void StudentWorld::createPea(int x, int y, int dir)  {
    int dx = 0;
    int dy = 0;
    switch (dir) { //gets direction
        case 90: // up
            dy = 1;
            break;
        case 270: //down
            dy = -1;
            break;
        case 180: //left
            dx = -1;
            break;
        case 0: // right
            dx = 1;
            break;
        default:
            break;
    }
    m_actors.push_back(new Peas(x+dx, y+dy, this, dir));
}


// Try to cause damage to something at a's location.  (a is only ever
// going to be a pea.)  Return true if something stops a -- something
// at this location prevents a pea from continuing.
bool StudentWorld::damageSomething(Actor* a){
    list<Actor*>::iterator it;
    it = m_actors.begin();
    while (it != m_actors.end()) {
        if ((*it)->getX() == a->getX() && (*it)->getY() == a->getY() && (*it)->isDamageable()) { //is damageable so like if its a wall shouldnt decrease hp
            (*it)->setHp(-2);
            if ((*it)->getHp() > 0 && (*it)->movingBot()){
                playSound(SOUND_ROBOT_IMPACT); //makes the proper sound if its still alive
            }
            else if ((*it)->getHp() <= 0 && (*it)->movingBot()) {
                if((*it)->returnThiefPickedUp()) {
                    int saveScore = (*it)->returnSaveGoodie(); //remembers the score to differentiate each goodie
                    switch (saveScore) {
                        case 500: //restorehealth goodie
                            m_actors.push_back(new RestoreHealthGoodie((*it)->getX(),(*it)->getY(),this, 500));
                            break;
                        case 1000: //extralife goodie
                            m_actors.push_back(new ExtraLifeGoodie((*it)->getX(),(*it)->getY(),this, 1000));
                            break;
                        case 100: // ammo goodie
                            m_actors.push_back(new AmmoGoodie((*it)->getX(),(*it)->getY(),this, 100));
                            break;
                        default:
                            break;
                    }
                }
                (*it)->setDead();
                playSound(SOUND_ROBOT_DIE);
                if ((*it)->isShootingRobot() && (*it)->canSteal()) {
                    increaseScore(20);
                }
                else if ((*it)->isShootingRobot()) {
                    increaseScore(100);
                }
                else {
                    increaseScore(10);
                }
            }
            //            if(!(*it)->isActive()) {
            //                (*it)->playSound(sou)
            a->setDead();
            return true;
        }
        else if ((*it)->getX() == a->getX() && (*it)->getY() == a->getY() && (*it)->stopsPea()) {
            a->setDead(); //set the pea to dead on impact
            return true;
        }
        it++;
    }
    return false;
}

// If a pea were at x,y moving in direction dx,dy, could it hit the
// player without encountering any obstructions?
//robots factories marbles walls blocks robots vision
bool StudentWorld::existsClearShotToPlayer(int x, int y, int dx, int dy) const {
    if ((dx < 0 && m_avatar->getX() < x && m_avatar->getY() == y)) { //checks direction in the same x axis
        list<Actor*>::const_iterator it;
        it = m_actors.begin();
        while (it != m_actors.end()) {
            if ((*it)->getX() < x && (*it)->getX() > m_avatar->getX() && (*it)->getY() == y) { //see if theres anythign in between like wall or marble
                if ((*it)->stopsRobotPea()) { //if the thing in bewtween blocks vision someitmes it doesnt like goodie
                    return false;
                }
            }
            it++;
        }
        return true;
    }
    else if ((dx > 0 && m_avatar->getX() > x && m_avatar->getY() == y)) {
        list<Actor*>::const_iterator it;
        it = m_actors.begin();
        while (it != m_actors.end()) {
            if ((*it)->getX() > x && (*it)->getX() < m_avatar->getX() && (*it)->getY() == y) {
                if ((*it)->stopsRobotPea()) {
                    return false;
                }
            }
            it++;
        }
        return true;
    }
    else if ((dy > 0 && m_avatar->getY() > y && m_avatar->getX() == x)) {
        list<Actor*>::const_iterator it;
        it = m_actors.begin();
        while (it != m_actors.end()) {
            if ((*it)->getY() > y && (*it)->getY() < m_avatar->getY() && (*it)->getX() == x) {
                if ((*it)->stopsRobotPea()) {
                    return false;
                }
            }
            it++;
        }
        return true;
    }
    else if ((dy < 0 && m_avatar->getY() < y && m_avatar->getX() == x)) {
        list<Actor*>::const_iterator it;
        it = m_actors.begin();
        while (it != m_actors.end()) {
            if ((*it)->getY() < y && (*it)->getY() > m_avatar->getY() && (*it)->getX() == x) {
                if ((*it)->stopsRobotPea()) {
                    return false;
                }
            }
            it++;
        }
        return true;
    }
    return false;
}

// Swallow any swallowable object at a's location.  (a is only ever
// going to be a pit.)
bool StudentWorld::swallowSwallowable(Actor* a) {
    list<Actor*>::iterator it;
    it = m_actors.begin();
    while (it != m_actors.end()) {
        if ((*it)->isPushable()) { //if its a marble
            if ((*it)->getX() == a->getX() && (*it)->getY() == a->getY()) {
                (*it)->setDead();
                a->setDead();
                return true;
            }
        }
        it++;
    }
    return false;
}

bool StudentWorld::isPlayerColocatedWith(int x, int y) const {
    if (m_avatar->getX() == x && m_avatar->getY() == y) { //is player on the same spot
        return true;
    }
    return false;
}

// Are there any crystals left on this level?
bool StudentWorld::anyCrystals() const {
    list<Actor*>::const_iterator it;
    it = m_actors.begin();
    while (it != m_actors.end()) {
        if ((*it)->opensExit()) { //looops through and checks for crytals
            return true;
        }
        it++;
    }
    return false;
}

// Reduce the count of crystals on this level by 1.
void StudentWorld::decCrystals()  {
    crystalCount--;
    crystalFlag = true;
}

// Restore player's health to the full amount.
void StudentWorld::restorePlayerHealth(){
    m_avatar->restoreHpPlayer();
}

// Increase the amount of ammunition the player has
void StudentWorld::increaseAmmo() {
    m_avatar->setPeas(20);
}


bool StudentWorld::validRobotWalk( int x, int y) {
    list<Actor*>:: const_iterator it;
    it = m_actors.begin();
    while( it != m_actors.end()) {
        
        if ((*it)->getX() == x && (*it)->getY() == y && (*it)->stopsRobot()) { //check if its not a valid walk as in x and y r
            return false;
        }
        it++;
    }
    return true; // filters the rest of the cases 1. if none x and y are equal move 2. if its equal but u can move in .
}

// If an item that can be stolen is at x,y, return a pointer to it;
// otherwise, return a null pointer.  (Stealable items are only ever
// going be goodies.)
Actor* StudentWorld::getColocatedStealable(int x, int y) const{
    list<Actor*>:: const_iterator it;
    it = m_actors.begin();
    while(it != m_actors.end()) {
        if((*it)->getX() == x && (*it)->getY() == y && (*it)->isStealable()) {
            return (*it);
        }
        it++;
    }
    return nullptr;
}

// If a factory is at x,y, how many items of the type that should be
// counted are in the rectangle bounded by x-distance,y-distance and
// x+distance,y+distance?  Set count to that number and return true,
// unless an item is on the factory itself, in which case return false
// and don't care about count.  (The items counted are only ever going
// ThiefBots.)
bool StudentWorld::doFactoryCensus(int x, int y, int distance, int& count) const {
    count = 0; //needs to reset count each time the function is called
    list<Actor*>:: const_iterator it;
    it = m_actors.begin();
    while(it != m_actors.end()) {
        if ((*it)->canSteal()) { //if its a thiefbot
            int allOfX = abs((*it)->getX() -x);//formula to calculate the 3 x 3 where the factory counts
            int allofY = abs((*it)->getY() -y);
            if (allOfX < 0) { //sets min and max
                allOfX = 0;
            }
            if (allofY < 0) {//sets min and max
                allofY = 0;
            }
            if (allOfX > 14) {
                allOfX = 14;
            }
            if (allofY > 14) {
                allofY = 14;
            }
            if (allOfX == 0 && allofY == 0) {
                count = 0;
                return false;
            }
            if (allOfX <= distance && allofY <= distance) {
                count++; //counts
            }
        }
        it++;
}
    return true;
}

bool StudentWorld::thiefFactoryColocation(int x, int y) const{
    list<Actor*>:: const_iterator it;
    it = m_actors.begin();
    while(it != m_actors.end()) {
        if ((*it)->getX() == x && (*it)->getY() == y && (*it)->canSteal()){ //if factroy and theifbot is the same
            return true;
        }
        it++;
    }
    return false;
}

void StudentWorld::makeThiefBots(int x, int y, int type) {
    list<Actor*>:: const_iterator it;
    it = m_actors.begin();
    while(it != m_actors.end()) {
        if((*it)->makesRobots()){
            if (type == 0) {
                m_actors.push_back(new RegularThiefBot(x, y, this)); //makes thiefbots based on 0 and 1 0 is regular 1 is mean
                playSound(SOUND_ROBOT_BORN);
            }
            if (type == 1){
                m_actors.push_back(new MeanThiefBot(x, y, this));
                playSound(SOUND_ROBOT_BORN);
            }
        }
        it++;
    }
    
}
